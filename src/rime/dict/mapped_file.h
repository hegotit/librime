//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-06-27 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_MAPPED_FILE_H_
#define RIME_MAPPED_FILE_H_

#include <stdint.h>
#include <cstring>
#include <rime_api.h>
#include <rime/common.h>

namespace rime {

// basic data structure

// Limitation: cannot point to itself (zero is used to represent NULL pointer)
template <class T = char, class Offset = int32_t>
class OffsetPtr {
 public:
  OffsetPtr() = default;
  explicit OffsetPtr(Offset offset) : offset_(offset) {}
  explicit OffsetPtr(const T* ptr) : OffsetPtr(to_offset(ptr)) {}
  explicit OffsetPtr(const OffsetPtr<T>& ptr) : OffsetPtr(ptr.get()) {}
  OffsetPtr<T>& operator=(const OffsetPtr<T>& ptr) {
    offset_ = to_offset(ptr.get());
    return *this;
  }
  OffsetPtr<T>& operator=(const T* ptr) {
    offset_ = to_offset(ptr);
    return *this;
  }
  explicit operator bool() const { return offset_; }
  T* operator->() const { return get(); }
  T& operator*() const { return *get(); }
  T& operator[](size_t index) const { return *(get() + index); }
  [[nodiscard]] T* get() const {
    if (!offset_)
      return nullptr;
    return reinterpret_cast<T*>((char*)&offset_ + offset_);
  }

 private:
  Offset to_offset(const T* ptr) const {
    return ptr ? (char*)ptr - (char*)(&offset_) : 0;
  }
  Offset offset_ = 0;
};

struct String {
  OffsetPtr<char> data;
  [[nodiscard]] const char* c_str() const { return data.get(); }
  [[nodiscard]] size_t length() const { return c_str() ? strlen(c_str()) : 0; }
  [[nodiscard]] bool empty() const { return !data || !data[0]; }
};

template <class T, class Size = uint32_t>
struct Array {
  Size size;
  T at[1];
  T* begin() { return &at[0]; }
  T* end() { return &at[0] + size; }
  const T* begin() const { return &at[0]; }
  const T* end() const { return &at[0] + size; }
};

template <class T, class Size = uint32_t>
struct List {
  Size size;
  OffsetPtr<T> at;
  T* begin() { return &at[0]; }
  T* end() { return &at[0] + size; }
  const T* begin() const { return &at[0]; }
  const T* end() const { return &at[0] + size; }
};

// MappedFile class definition

class MappedFileImpl;

class RIME_API MappedFile {
 protected:
  explicit MappedFile(path  file_path);
  virtual ~MappedFile();

  bool Create(size_t capacity);
  bool OpenReadOnly();
  bool OpenReadWrite();
  bool Flush();
  bool Resize(size_t capacity);
  bool ShrinkToFit();

  template <class T>
  T* Allocate(size_t count = 1);

  template <class T>
  Array<T>* CreateArray(size_t array_size);

  String* CreateString(const string& str);
  bool CopyString(const string& src, String* dest);

  [[nodiscard]] size_t capacity() const;
  [[nodiscard]] char* address() const;

 public:
  // noncpyable
  MappedFile(const MappedFile&) = delete;
  MappedFile& operator=(const MappedFile&) = delete;

  [[nodiscard]] bool Exists() const;
  [[nodiscard]] bool IsOpen() const;
  void Close();
  bool Remove();

  template <class T>
  T* Find(size_t offset);

  [[nodiscard]] const path& file_path() const { return file_path_; }
  [[nodiscard]] size_t file_size() const { return size_; }

 private:
  path file_path_;
  size_t size_ = 0;
  the<MappedFileImpl> file_;
};

// member function definitions

#define RIME_ALIGNED(size, T) ((size + alignof(T) - 1) & ~(alignof(T) - 1))

template <class T>
T* MappedFile::Allocate(size_t count) {
  if (!IsOpen())
    return NULL;

  size_t used_space = RIME_ALIGNED(size_, T);
  size_t required_space = sizeof(T) * count;
  size_t file_size = capacity();
  if (used_space + required_space > file_size) {
    // not enough space; grow the file
    size_t new_size = (std::max)(used_space + required_space, file_size * 2);
    if (!Resize(new_size) || !OpenReadWrite())
      return NULL;
  }
  T* ptr = reinterpret_cast<T*>(address() + used_space);
  std::memset(ptr, 0, required_space);
  size_ = used_space + required_space;
  return ptr;
}

template <class T>
T* MappedFile::Find(size_t offset) {
  if (!IsOpen() || offset > size_)
    return NULL;
  return reinterpret_cast<T*>(address() + offset);
}

template <class T>
Array<T>* MappedFile::CreateArray(size_t array_size) {
  size_t num_bytes = sizeof(Array<T>) + sizeof(T) * (array_size - 1);
  auto* ret = reinterpret_cast<Array<T>*>(Allocate<char>(num_bytes));
  if (!ret)
    return NULL;
  ret->size = array_size;
  return ret;
}

}  // namespace rime

#endif  // RIME_MAPPED_FILE_H_

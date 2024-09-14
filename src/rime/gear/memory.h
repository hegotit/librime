//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2013-01-02 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_MEMORY_H_
#define RIME_MEMORY_H_

#include <rime/common.h>
#include <rime/dict/vocabulary.h>

namespace rime {

class KeyEvent;
class Context;
class Engine;
class Dictionary;
class UserDictionary;
class Language;
class Phrase;
class Memory;

struct CommitEntry : DictEntry {
  vector<const DictEntry*> elements;
  Memory* memory;

  explicit CommitEntry(Memory* a_memory = nullptr) : memory(a_memory) {}
  [[nodiscard]] bool empty() const { return text.empty(); }
  void Clear();
  void AppendPhrase(const an<Phrase>& phrase);
  [[nodiscard]] bool Save() const;
};

class Memory {
 public:
  explicit Memory(const Ticket& ticket);
  virtual ~Memory();

  virtual bool Memorize(const CommitEntry& commit_entry) = 0;

  bool StartSession();
  bool FinishSession();
  bool DiscardSession();

  [[nodiscard]] Dictionary* dict() const { return dict_.get(); }
  [[nodiscard]] UserDictionary* user_dict() const { return user_dict_.get(); }

  [[nodiscard]] const Language* language() const { return language_.get(); }

 protected:
  void OnCommit(Context* ctx);
  void OnDeleteEntry(Context* ctx);
  void OnUnhandledKey(Context* ctx, const KeyEvent& key);

  the<Dictionary> dict_;
  the<UserDictionary> user_dict_;
  the<Language> language_;

 private:
  connection commit_connection_;
  connection delete_connection_;
  connection unhandled_key_connection_;
};

}  // namespace rime

#endif  // RIME_MEMORY_H_

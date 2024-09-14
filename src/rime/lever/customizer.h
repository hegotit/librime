//
// Copyright RIME Developers
// Distributed under the BSD License
//
#ifndef RIME_CUSTOMIZER_H_
#define RIME_CUSTOMIZER_H_

#include <rime/common.h>

#include <utility>

namespace rime {

class Customizer {
 public:
  Customizer(path  source_path,
             path  dest_path,
             string  version_key)
      : source_path_(std::move(source_path)),
        dest_path_(std::move(dest_path)),
        version_key_(std::move(version_key)) {}

  // DEPRECATED: in favor of auto-patch config compiler plugin
  bool UpdateConfigFile();

 protected:
  path source_path_;
  path dest_path_;
  string version_key_;
};

}  // namespace rime

#endif  // RIME_CUSTOMIZER_H_

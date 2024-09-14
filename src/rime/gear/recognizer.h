//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2012-01-01 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_RECOGNIZER_H_
#define RIME_RECOGNIZER_H_

#include <boost/regex.hpp>
#include <utility>
#include <rime/common.h>
#include <rime/processor.h>

namespace rime {

class Config;
class Segmentation;

struct RecognizerMatch {
  string tag;
  size_t start = 0, end = 0;

  RecognizerMatch() = default;
  RecognizerMatch(string  a_tag, size_t a_start, size_t an_end)
      : tag(std::move(a_tag)), start(a_start), end(an_end) {}

  [[nodiscard]] bool found() const { return start < end; }
};

class RecognizerPatterns : public map<string, boost::regex> {
 public:
  void LoadConfig(Config* config, const string& name_space);
  [[nodiscard]] RecognizerMatch GetMatch(const string& input,
                           const Segmentation& segmentation) const;
};

class Recognizer : public Processor {
 public:
  explicit Recognizer(const Ticket& ticket);

  ProcessResult ProcessKeyEvent(const KeyEvent& key_event) override;

 protected:
  RecognizerPatterns patterns_;
  bool use_space_ = false;
};

}  // namespace rime

#endif  // RIME_RECOGNIZER_H_

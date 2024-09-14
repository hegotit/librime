//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-11-21 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_PUNCTUATOR_H_
#define RIME_PUNCTUATOR_H_

#include <rime/common.h>
#include <rime/component.h>
#include <rime/config.h>
#include <rime/processor.h>
#include <rime/segmentor.h>
#include <rime/translator.h>

namespace rime {

class Engine;

class PunctConfig {
 public:
  void LoadConfig(Engine* engine, bool load_symbols = false);
  an<ConfigItem> GetPunctDefinition(const string& key);

 protected:
  string shape_;
  an<ConfigMap> mapping_;
  an<ConfigMap> symbols_;
};

class Punctuator : public Processor {
 public:
  explicit Punctuator(const Ticket& ticket);
  ProcessResult ProcessKeyEvent(const KeyEvent& key_event) override;

 protected:
  bool ConfirmUniquePunct(const an<ConfigItem>& definition);
  bool AlternatePunct(const string& key, const an<ConfigItem>& definition);
  bool AutoCommitPunct(const an<ConfigItem>& definition);
  bool PairPunct(const an<ConfigItem>& definition);

  PunctConfig config_;
  bool use_space_ = false;
  map<an<ConfigItem>, int> oddness_;
};

class PunctSegmentor : public Segmentor {
 public:
  explicit PunctSegmentor(const Ticket& ticket);
  bool Proceed(Segmentation* segmentation) override;

 protected:
  PunctConfig config_;
};

class PunctTranslator : public Translator {
 public:
  explicit PunctTranslator(const Ticket& ticket);
  an<Translation> Query(const string& input, const Segment& segment) override;

 protected:
  static an<Translation> TranslateUniquePunct(const string& key,
                                       const Segment& segment,
                                       const an<ConfigValue>& definition);
  static an<Translation> TranslateAlternatingPunct(const string& key,
                                            const Segment& segment,
                                            const an<ConfigList>& definition);
  static an<Translation> TranslateAutoCommitPunct(const string& key,
                                           const Segment& segment,
                                           const an<ConfigMap>& definition);
  static an<Translation> TranslatePairedPunct(const string& key,
                                       const Segment& segment,
                                       const an<ConfigMap>& definition);

  PunctConfig config_;
};

}  // namespace rime

#endif  // RIME_PUNCTUATOR_H_

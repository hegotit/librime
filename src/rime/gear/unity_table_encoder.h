//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2013-08-31 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_UNITY_TABLE_ENCODER_H_
#define RIME_UNITY_TABLE_ENCODER_H_

#include <rime/common.h>
#include <rime/algo/encoder.h>

namespace rime {

struct Ticket;
class ReverseLookupDictionary;
class UserDictionary;

class UnityTableEncoder : public TableEncoder, public PhraseCollector {
 public:
  explicit UnityTableEncoder(UserDictionary* user_dict);
  ~UnityTableEncoder() override;

  bool Load(const Ticket& ticket);

  void CreateEntry(const string& word,
                   const string& code_str,
                   const string& weight_str) override;
  bool TranslateWord(const string& word, vector<string>* code) override;

  size_t LookupPhrases(UserDictEntryIterator* result,
                       const string& input,
                       bool predictive,
                       size_t limit = 0,
                       string* resume_key = nullptr);

  static bool HasPrefix(const string& key);
  static bool AddPrefix(string* key);
  static bool RemovePrefix(string* key);

 protected:
  UserDictionary* user_dict_;
  the<ReverseLookupDictionary> rev_dict_;
};

}  // namespace rime

#endif  // RIME_UNITY_TABLE_ENCODER_H_

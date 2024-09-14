//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2012-04-22 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_TRANSLATOR_COMMONS_H_
#define RIME_TRANSLATOR_COMMONS_H_

#include <boost/regex.hpp>
#include <rime/common.h>
#include <rime/config.h>
#include <rime/candidate.h>
#include <rime/translation.h>
#include <rime/algo/algebra.h>
#include <rime/algo/syllabifier.h>
#include <rime/dict/vocabulary.h>

namespace rime {

//

class Patterns : public vector<boost::regex> {
 public:
  bool Load(const an<ConfigList>& patterns);
};

//

class Spans {
 public:
  void AddVertex(size_t vertex);
  void AddSpan(size_t start, size_t end);
  void AddSpans(const Spans& spans);
  void Clear();
  // move by syllable by returning a value different from caret_pos
  [[nodiscard]] size_t PreviousStop(size_t caret_pos) const;
  [[nodiscard]] size_t NextStop(size_t caret_pos) const;
  [[nodiscard]] size_t Count(size_t start_pos, size_t end_pos) const;
  [[nodiscard]] size_t Count() const { return vertices_.empty() ? 0 : vertices_.size() - 1; }
  [[nodiscard]] size_t start() const { return vertices_.empty() ? 0 : vertices_.front(); }
  [[nodiscard]] size_t end() const { return vertices_.empty() ? 0 : vertices_.back(); }
  [[nodiscard]] bool HasVertex(size_t vertex) const;
  void set_vertices(vector<size_t>&& vertices) { vertices_ = vertices; }

 private:
  vector<size_t> vertices_;
};

class Phrase;

class PhraseSyllabifier {
 public:
  virtual ~PhraseSyllabifier() = default;

  virtual Spans Syllabify(const Phrase* phrase) = 0;
};

//

class Language;

class Phrase : public Candidate {
 public:
  Phrase(const Language* language,
         const string& type,
         size_t start,
         size_t end,
         const an<DictEntry>& entry)
      : Candidate(type, start, end), language_(language), entry_(entry) {}
  [[nodiscard]] const string& text() const override { return entry_->text; }
  [[nodiscard]] string comment() const override { return entry_->comment; }
  [[nodiscard]] string preedit() const override { return entry_->preedit; }
  void set_comment(const string& comment) { entry_->comment = comment; }
  void set_preedit(const string& preedit) { entry_->preedit = preedit; }
  void set_syllabifier(an<PhraseSyllabifier> syllabifier) {
    syllabifier_ = std::move(syllabifier);
  }
  [[nodiscard]] double weight() const { return entry_->weight; }
  void set_weight(double weight) { entry_->weight = weight; }
  [[nodiscard]] Code& code() const { return entry_->code; }
  [[nodiscard]] const DictEntry& entry() const { return *entry_; }
  [[nodiscard]] const Language* language() const { return language_; }
  [[nodiscard]] size_t matching_code_size() const {
    return entry_->matching_code_size != 0 ? entry_->matching_code_size
                                           : entry_->code.size();
  }
  [[nodiscard]] bool is_exact_match() const {
    return entry_->matching_code_size == 0 ||
           entry_->matching_code_size == entry_->code.size();
  }
  [[nodiscard]] bool is_predicitve_match() const {
    return entry_->matching_code_size != 0 &&
           entry_->matching_code_size < entry_->code.size();
  }
  [[nodiscard]] Code matching_code() const {
    return entry_->matching_code_size == 0
               ? entry_->code
               : Code(entry_->code.begin(),
                      entry_->code.begin() + entry_->matching_code_size);
  }
  Spans spans() {
    return syllabifier_ ? syllabifier_->Syllabify(this) : Spans();
  }

 protected:
  const Language* language_;
  an<DictEntry> entry_;
  an<PhraseSyllabifier> syllabifier_;
};

//

class Sentence : public Phrase {
 public:
  explicit Sentence(const Language* language)
      : Phrase(language, "sentence", 0, 0, New<DictEntry>()) {}
  Sentence(const Sentence& other)
      : Phrase(other),
        components_(other.components_),
        word_lengths_(other.word_lengths_) {
    entry_ = New<DictEntry>(other.entry());
  }
  void Extend(const DictEntry& another, size_t end_pos, double new_weight);
  void Offset(size_t offset);

  [[nodiscard]] bool empty() const { return components_.empty(); }

  [[nodiscard]] size_t size() const { return components_.size(); }

  [[nodiscard]] const vector<DictEntry>& components() const { return components_; }
  [[nodiscard]] const vector<size_t>& word_lengths() const { return word_lengths_; }

 protected:
  vector<DictEntry> components_;
  vector<size_t> word_lengths_;
};

//

struct Ticket;

class TranslatorOptions {
 public:
  explicit TranslatorOptions(const Ticket& ticket);
  [[nodiscard]] bool IsUserDictDisabledFor(const string& input) const;

  [[nodiscard]] const string& delimiters() const { return delimiters_; }
  [[nodiscard]] vector<string> tags() const { return tags_; }
  void set_tags(const vector<string>& tags) {
    tags_ = tags;
    if (tags_.empty()) {
      tags_.emplace_back("abc");
    }
  }
  [[nodiscard]] const string& tag() const { return tags_[0]; }
  void set_tag(const string& tag) { tags_[0] = tag; }
  [[nodiscard]] bool contextual_suggestions() const { return contextual_suggestions_; }
  void set_contextual_suggestions(bool enabled) {
    contextual_suggestions_ = enabled;
  }
  [[nodiscard]] bool enable_completion() const { return enable_completion_; }
  void set_enable_completion(bool enabled) { enable_completion_ = enabled; }
  [[nodiscard]] bool strict_spelling() const { return strict_spelling_; }
  void set_strict_spelling(bool is_strict) { strict_spelling_ = is_strict; }
  [[nodiscard]] double initial_quality() const { return initial_quality_; }
  void set_initial_quality(double quality) { initial_quality_ = quality; }
  Projection& preedit_formatter() { return preedit_formatter_; }
  Projection& comment_formatter() { return comment_formatter_; }

 protected:
  string delimiters_;
  vector<string> tags_{"abc"};  // invariant: non-empty
  bool contextual_suggestions_ = false;
  bool enable_completion_ = true;
  bool strict_spelling_ = false;
  double initial_quality_ = 0.;
  Projection preedit_formatter_;
  Projection comment_formatter_;
  Patterns user_dict_disabling_patterns_;
};

}  // namespace rime

#endif  // RIME_TRANSLATOR_COMMONS_H_

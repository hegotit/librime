//
// Copyright RIME Developers
// Distributed under the BSD License
//

#include <rime/common.h>
#include <rime/translation.h>

#include <utility>

namespace rime {

class Candidate;
class Grammar;
class Phrase;

class ContextualTranslation : public PrefetchTranslation {
 public:
  ContextualTranslation(an<Translation> translation,
                        string input,
                        string preceding_text,
                        Grammar* grammar)
      : PrefetchTranslation(std::move(translation)),
        input_(std::move(input)),
        preceding_text_(std::move(preceding_text)),
        grammar_(grammar) {}

 protected:
  bool Replenish() override;

 private:
  an<Phrase> Evaluate(an<Phrase> phrase);
  void AppendToCache(vector<of<Phrase>>& queue);

  string input_;
  string preceding_text_;
  Grammar* grammar_;
};

}  // namespace rime

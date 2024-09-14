//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-7-12 Zou xu <zouivex@gmail.com>
//

#ifndef RIME_SYLLABIFIER_H_
#define RIME_SYLLABIFIER_H_

#include <stdint.h>
#include <rime_api.h>

#include <utility>
#include "spelling.h"

namespace rime {

class Prism;
class Corrector;

using SyllableId = int32_t;

struct EdgeProperties : SpellingProperties {
  explicit EdgeProperties(SpellingProperties sup) : SpellingProperties(std::move(sup)) {};
  EdgeProperties() = default;
  bool is_correction = false;
};

using SpellingMap = map<SyllableId, EdgeProperties>;
using VertexMap = map<size_t, SpellingType>;
using EndVertexMap = map<size_t, SpellingMap>;
using EdgeMap = map<size_t, EndVertexMap>;

using SpellingPropertiesList = vector<const EdgeProperties*>;
using SpellingIndex = map<SyllableId, SpellingPropertiesList>;
using SpellingIndices = map<size_t, SpellingIndex>;

struct SyllableGraph {
  size_t input_length = 0;
  size_t interpreted_length = 0;
  VertexMap vertices;
  EdgeMap edges;
  SpellingIndices indices;
};

class Syllabifier {
 public:
  Syllabifier() = default;
  explicit Syllabifier(string delimiters,
                       bool enable_completion = false,
                       bool strict_spelling = false)
      : delimiters_(std::move(delimiters)),
        enable_completion_(enable_completion),
        strict_spelling_(strict_spelling) {}

  RIME_API int BuildSyllableGraph(const string& input,
                                  Prism& prism,
                                  SyllableGraph* graph);
  RIME_API void EnableCorrection(Corrector* corrector);

 protected:
  static void CheckOverlappedSpellings(SyllableGraph* graph, size_t start, size_t end);
  static void Transpose(SyllableGraph* graph);

  string delimiters_;
  bool enable_completion_ = false;
  bool strict_spelling_ = false;
  Corrector* corrector_ = nullptr;
};

}  // namespace rime

#endif  // RIME_SYLLABIFIER_H_

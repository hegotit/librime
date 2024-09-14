//
// Copyright RIME Developers
// Distributed under the BSD License
//
#ifndef RIME_CONFIG_COMPILER_IMPL_H_
#define RIME_CONFIG_COMPILER_IMPL_H_

#include <ostream>
#include <utility>
#include <rime/common.h>
#include <rime/config/config_compiler.h>
#include <rime/config/config_types.h>

namespace rime {

enum DependencyPriority {
  kPendingChild = 0,
  kInclude = 1,
  kPatch = 2,
};

struct Dependency {
  an<ConfigItemRef> target;

  [[nodiscard]] virtual DependencyPriority priority() const = 0;
  [[nodiscard]] bool blocking() const { return priority() > kPendingChild; }
  [[nodiscard]] virtual string repr() const = 0;
  Dependency& TargetedAt(an<ConfigItemRef> target) {
    this->target = std::move(target);
    return *this;
  }
  virtual bool Resolve(ConfigCompiler* compiler) = 0;
};

std::ostream& operator<<(std::ostream& stream, const Dependency& dependency);

struct PendingChild : Dependency {
  string child_path;
  an<ConfigItemRef> child_ref;

  PendingChild(string  path, const an<ConfigItemRef>& ref)
      : child_path(std::move(path)), child_ref(ref) {}
  [[nodiscard]] DependencyPriority priority() const override { return kPendingChild; }
  [[nodiscard]] string repr() const override { return "PendingChild(" + child_path + ")"; }
  bool Resolve(ConfigCompiler* compiler) override;
};

struct IncludeReference : Dependency {
  Reference reference;

  explicit IncludeReference(Reference  r) : reference(std::move(r)) {}
  [[nodiscard]] DependencyPriority priority() const override { return kInclude; }
  [[nodiscard]] string repr() const override { return "Include(" + reference.repr() + ")"; }
  bool Resolve(ConfigCompiler* compiler) override;
};

struct PatchReference : Dependency {
  Reference reference;

  explicit PatchReference(Reference  r) : reference(std::move(r)) {}
  [[nodiscard]] DependencyPriority priority() const override { return kPatch; }
  [[nodiscard]] string repr() const override { return "Patch(" + reference.repr() + ")"; }
  bool Resolve(ConfigCompiler* compiler) override;
};

struct PatchLiteral : Dependency {
  an<ConfigMap> patch;

  explicit PatchLiteral(an<ConfigMap> map) : patch(std::move(map)) {}
  [[nodiscard]] DependencyPriority priority() const override { return kPatch; }
  [[nodiscard]] string repr() const override { return "Patch(<literal>)"; }
  bool Resolve(ConfigCompiler* compiler) override;
};

}  // namespace rime

#endif  // RIME_CONFIG_COMPILER_IMPL_H_

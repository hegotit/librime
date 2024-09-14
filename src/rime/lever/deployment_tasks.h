//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2012-02-10 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_DEPLOYMENT_TASKS_H_
#define RIME_DEPLOYMENT_TASKS_H_

#include <rime/common.h>
#include <rime/deployer.h>

#include <utility>

namespace rime {

// detects changes in either user configuration or upgraded shared data
class DetectModifications : public DeploymentTask {
 public:
  explicit DetectModifications(TaskInitializer arg = TaskInitializer());
  // Unlike other tasks, its return value indicates whether modifications
  // has been detected and workspace needs update.
  bool Run(Deployer* deployer) override;

 protected:
  vector<path> data_dirs_;
};

// initialize/update installation.yaml
class RIME_API InstallationUpdate : public DeploymentTask {
 public:
  explicit InstallationUpdate(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

// update distributed config files and preset schemas
class RIME_API WorkspaceUpdate : public DeploymentTask {
 public:
  explicit WorkspaceUpdate(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;

 protected:
  string GetSchemaPath(Deployer* deployer,
                       const string& schema_id,
                       bool prefer_shared_copy);
};

// update a specific schema, build corresponding dictionary
class RIME_API SchemaUpdate : public DeploymentTask {
 public:
  explicit SchemaUpdate(path  source_path) : source_path_(std::move(source_path)) {}
  explicit SchemaUpdate(TaskInitializer arg);
  bool Run(Deployer* deployer) override;
  void set_verbose(bool verbose) { verbose_ = verbose; }

 protected:
  path source_path_;
  bool verbose_ = false;
};

// update a specific config file
class ConfigFileUpdate : public DeploymentTask {
 public:
  ConfigFileUpdate(string file_name, string version_key)
      : file_name_(std::move(file_name)),
        version_key_(std::move(version_key)) {}
  explicit ConfigFileUpdate(TaskInitializer arg);
  bool Run(Deployer* deployer) override;

 protected:
  string file_name_;
  string version_key_;
};

// for installer
class PrebuildAllSchemas : public DeploymentTask {
 public:
  explicit PrebuildAllSchemas(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

// create symlinks to prebuilt dictionaries in user directory
class SymlinkingPrebuiltDictionaries : public DeploymentTask {
 public:
  explicit SymlinkingPrebuiltDictionaries(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

// upgrade user dictionaries
class UserDictUpgrade : public DeploymentTask {
 public:
  explicit UserDictUpgrade(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

class UserDictSync : public DeploymentTask {
 public:
  explicit UserDictSync(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

class BackupConfigFiles : public DeploymentTask {
 public:
  explicit BackupConfigFiles(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

class CleanupTrash : public DeploymentTask {
 public:
  explicit CleanupTrash(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

class CleanOldLogFiles : public DeploymentTask {
 public:
  explicit CleanOldLogFiles(TaskInitializer arg = TaskInitializer()) {}
  bool Run(Deployer* deployer) override;
};

}  // namespace rime

#endif  // RIME_DEPLOYMENT_TASKS_H_

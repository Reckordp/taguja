#ifndef SRC_NODE_BUILTINS_H_
#define SRC_NODE_BUILTINS_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "node_mutex.h"
#include "node_union_bytes.h"
#include "v8.h"

// Forward declare test fixture for `friend` declaration.
class PerProcessTest;

namespace node {
class SnapshotBuilder;
class ExternalReferenceRegistry;
namespace builtins {

using BuiltinSourceMap = std::map<std::string, UnionBytes>;
using BuiltinCodeCacheMap =
    std::unordered_map<std::string,
                       std::unique_ptr<v8::ScriptCompiler::CachedData>>;

struct CodeCacheInfo {
  std::string id;
  std::vector<uint8_t> data;
};

// Handles compilation and caching of built-in JavaScript modules and
// bootstrap scripts, whose source are bundled into the binary as static data.
class NODE_EXTERN_PRIVATE BuiltinLoader {
 public:
  BuiltinLoader(const BuiltinLoader&) = delete;
  BuiltinLoader& operator=(const BuiltinLoader&) = delete;

  static void RegisterExternalReferences(ExternalReferenceRegistry* registry);
  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context,
                         void* priv);

  // The parameters used to compile the scripts are detected based on
  // the pattern of the id.
  static v8::MaybeLocal<v8::Function> LookupAndCompile(
      v8::Local<v8::Context> context,
      const char* id,
      Environment* optional_env);

  static v8::Local<v8::Object> GetSourceObject(v8::Local<v8::Context> context);
  // Returns config.gypi as a JSON string
  static v8::Local<v8::String> GetConfigString(v8::Isolate* isolate);
  static bool Exists(const char* id);
  static bool Add(const char* id, const UnionBytes& source);
  static bool Add(const char* id, std::string_view utf8source);

  static bool CompileAllBuiltins(v8::Local<v8::Context> context);
  static void RefreshCodeCache(const std::vector<CodeCacheInfo>& in);
  static void CopyCodeCache(std::vector<CodeCacheInfo>* out);

  // pindah
  static BuiltinLoader* GetInstance();
  std::vector<std::string> GetBuiltinIds();

 private:
  // Only allow access from friends.
  friend class CodeCacheBuilder;

  BuiltinLoader();

  // Generated by tools/js2c.py as node_javascript.cc
  void LoadJavaScriptSource();  // Loads data into source_
  UnionBytes GetConfig();       // Return data for config.gypi

  struct BuiltinCategories {
    bool is_initialized = false;
    std::set<std::string> can_be_required;
    std::set<std::string> cannot_be_required;
  };
  void InitializeBuiltinCategories();
  const std::set<std::string>& GetCannotBeRequired();
  const std::set<std::string>& GetCanBeRequired();

  bool CanBeRequired(const char* id);
  bool CannotBeRequired(const char* id);

  BuiltinCodeCacheMap* code_cache();
  const Mutex& code_cache_mutex() const { return code_cache_mutex_; }

  v8::ScriptCompiler::CachedData* GetCodeCache(const char* id) const;
  enum class Result { kWithCache, kWithoutCache };
  v8::MaybeLocal<v8::String> LoadBuiltinSource(v8::Isolate* isolate,
                                               const char* id);
  // If an exception is encountered (e.g. source code contains
  // syntax error), the returned value is empty.
  v8::MaybeLocal<v8::Function> LookupAndCompileInternal(
      v8::Local<v8::Context> context,
      const char* id,
      std::vector<v8::Local<v8::String>>* parameters,
      Result* result);

  static void RecordResult(const char* id,
                           BuiltinLoader::Result result,
                           Environment* env);
  static void GetBuiltinCategories(
      v8::Local<v8::Name> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetCacheUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
  // Passing ids of built-in source code into JS land as
  // internalBinding('builtins').builtinIds
  static void BuiltinIdsGetter(v8::Local<v8::Name> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
  // Passing config.gypi into JS land as internalBinding('builtins').config
  static void ConfigStringGetter(
      v8::Local<v8::Name> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  // Compile a specific built-in as a function
  static void CompileFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void HasCachedBuiltins(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  static void AddExternalizedBuiltin(const char* id, const char* filename);

  static BuiltinLoader instance_;
  BuiltinCategories builtin_categories_;
  BuiltinSourceMap source_;
  BuiltinCodeCacheMap code_cache_;
  UnionBytes config_;

  // Used to synchronize access to the code cache map
  Mutex code_cache_mutex_;

  bool has_code_cache_;

  friend class ::PerProcessTest;
};
}  // namespace builtins

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_NODE_BUILTINS_H_

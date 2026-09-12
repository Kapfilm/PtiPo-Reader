#pragma once
// Reuse the real-file shim, mapping device absolute paths into an isolated test root.
#define HalStorage BaseTestStorage
#include "../zip_entry_reader/HalStorage.h"
#undef HalStorage
#undef Storage
class ClippingTestStorage {
 public:
  std::string root;
  std::string path(const std::string& p) const { return root + p; }
  bool openFileForRead(const char*, const std::string& p, FsFile& f) { return f.openForRead(path(p)); }
  bool openFileForWrite(const char*, const std::string& p, FsFile& f) { return f.openForWrite(path(p)); }
  bool exists(const char* p) { return std::filesystem::exists(path(p)); }
  bool mkdir(const char* p) { return std::filesystem::create_directories(path(p)); }
  bool remove(const char* p) { return std::filesystem::remove(path(p)); }
  bool rename(const char* a, const char* b) {
    std::error_code ec;
    std::filesystem::rename(path(a), path(b), ec);
    return !ec;
  }
  FsFile open(const char* p, int flags) {
    FsFile f;
    if (flags & O_WRONLY)
      f.openForWrite(path(p));
    else
      f.openForRead(path(p));
    return f;
  }
};
inline ClippingTestStorage clippingTestStorage;
#define Storage clippingTestStorage

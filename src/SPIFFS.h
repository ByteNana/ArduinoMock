#pragma once

#include <cstdint>
#include <map>
#include <string>

#include "WString.h"

#define FILE_WRITE "w"
#define FILE_READ "r"
#define FILE_APPEND "a"

class File {
 public:
  File() = default;
  File(std::string* contentPtr, const std::string& name, const std::string& mode);

  operator bool() const;
  String readString();
  String readStringUntil(char terminator);
  size_t write(uint8_t c);
  size_t write(const uint8_t* buf, size_t size);
  size_t print(const char* str);
  size_t println(const char* str);
  int read();
  int peek();
  int available();
  String name();
  size_t size();
  void close();

 private:
  std::string* _contentPtr = nullptr;
  std::string _name;
  std::string _mode;
  size_t _readPos = 0;
  bool _open = false;
};

class MockSPIFFS {
 public:
  bool begin(bool formatOnFail = false);
  File open(const char* path, const char* mode = FILE_READ);
  bool exists(const char* path);
  bool remove(const char* path);
  bool rename(const char* from, const char* to);

  // Test helpers
  void setMounted(bool v) { _mounted = v; }
  void addFile(const char* path, const std::string& content) { _files[path] = content; }
  std::string getFile(const char* path) { return _files.count(path) ? _files[path] : ""; }
  void clear() { _files.clear(); }
  void reset() {
    _files.clear();
    _mounted = true;
  }

 private:
  std::map<std::string, std::string> _files;
  bool _mounted = true;
  friend class File;
};

extern MockSPIFFS SPIFFS;

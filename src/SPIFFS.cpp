#include "SPIFFS.h"

#include <cstring>

// --- File implementation ---

File::File(std::string* contentPtr, const std::string& name, const std::string& mode)
    : _contentPtr(contentPtr), _name(name), _mode(mode), _readPos(0), _open(true) {}

File::operator bool() const { return _open && _contentPtr != nullptr; }

String File::readString() {
  if (!_contentPtr) return String("");
  return String(_contentPtr->c_str());
}

size_t File::write(uint8_t c) {
  if (!_contentPtr) return 0;
  _contentPtr->push_back(static_cast<char>(c));
  return 1;
}

size_t File::write(const uint8_t* buf, size_t size) {
  if (!_contentPtr || !buf) return 0;
  _contentPtr->append(reinterpret_cast<const char*>(buf), size);
  return size;
}

size_t File::print(const char* str) {
  if (!_contentPtr || !str) return 0;
  size_t len = strlen(str);
  _contentPtr->append(str, len);
  return len;
}

size_t File::println(const char* str) {
  size_t n = print(str);
  if (_contentPtr) {
    _contentPtr->push_back('\n');
    n++;
  }
  return n;
}

int File::read() {
  if (!_contentPtr || _readPos >= _contentPtr->size()) return -1;
  return static_cast<uint8_t>((*_contentPtr)[_readPos++]);
}

int File::peek() {
  if (!_contentPtr || _readPos >= _contentPtr->size()) return -1;
  return static_cast<uint8_t>((*_contentPtr)[_readPos]);
}

int File::available() {
  if (!_contentPtr) return 0;
  if (_readPos >= _contentPtr->size()) return 0;
  return static_cast<int>(_contentPtr->size() - _readPos);
}

String File::name() { return String(_name.c_str()); }

size_t File::size() { return _contentPtr ? _contentPtr->size() : 0; }

void File::close() {
  _open = false;
  _contentPtr = nullptr;
}

// --- MockSPIFFS implementation ---

bool MockSPIFFS::begin(bool) { return _mounted; }

File MockSPIFFS::open(const char* path, const char* mode) {
  std::string m = mode ? mode : FILE_READ;

  if (m == FILE_WRITE) {
    _files[path] = "";
    return File(&_files[path], path, m);
  }

  if (m == FILE_APPEND) {
    if (!_files.count(path)) _files[path] = "";
    return File(&_files[path], path, m);
  }

  // Read mode
  if (!_files.count(path)) return File();
  return File(&_files[path], path, m);
}

bool MockSPIFFS::exists(const char* path) { return _files.count(path) > 0; }

bool MockSPIFFS::remove(const char* path) {
  if (!_files.count(path)) return false;
  _files.erase(path);
  return true;
}

bool MockSPIFFS::rename(const char* from, const char* to) {
  if (!_files.count(from)) return false;
  _files[to] = _files[from];
  _files.erase(from);
  return true;
}

MockSPIFFS SPIFFS;
MockSPIFFS LittleFS;

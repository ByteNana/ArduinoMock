#pragma once
#include <cstring>
#include <map>
#include <string>

class Preferences {
 public:
  bool begin(const char* name, bool readOnly = false) {
    _ns = name;
    return true;
  }
  void end() {}
  void clear() { _store.clear(); }
  bool remove(const char* key) { return _store.erase(key) > 0; }

  bool putBool(const char* key, bool val) { return putBytes(key, &val, sizeof(val)); }
  bool getBool(const char* key, bool def = false) {
    bool v = def;
    getBytes(key, &v, sizeof(v));
    return v;
  }

  bool putInt(const char* key, int32_t val) { return putBytes(key, &val, sizeof(val)); }
  int32_t getInt(const char* key, int32_t def = 0) {
    int32_t v = def;
    getBytes(key, &v, sizeof(v));
    return v;
  }

  bool putUInt(const char* key, uint32_t val) { return putBytes(key, &val, sizeof(val)); }
  uint32_t getUInt(const char* key, uint32_t def = 0) {
    uint32_t v = def;
    getBytes(key, &v, sizeof(v));
    return v;
  }

  bool putFloat(const char* key, float val) { return putBytes(key, &val, sizeof(val)); }
  float getFloat(const char* key, float def = 0.0f) {
    float v = def;
    getBytes(key, &v, sizeof(v));
    return v;
  }

  bool putString(const char* key, const char* val) {
    _store[key] = std::string(val);
    return true;
  }
  size_t getString(const char* key, char* buf, size_t maxLen) {
    auto it = _store.find(key);
    if (it == _store.end() || buf == nullptr) return 0;
    size_t len = it->second.size();
    size_t toCopy = (len + 1 < maxLen) ? len + 1 : maxLen;
    memcpy(buf, it->second.c_str(), toCopy);
    if (maxLen > 0) buf[maxLen - 1] = '\0';
    return len;
  }

  bool putBytes(const char* key, const void* buf, size_t len) {
    _store[key] = std::string(reinterpret_cast<const char*>(buf), len);
    return true;
  }
  size_t getBytes(const char* key, void* buf, size_t maxLen) {
    auto it = _store.find(key);
    if (it == _store.end() || buf == nullptr) return 0;
    size_t toCopy = (it->second.size() < maxLen) ? it->second.size() : maxLen;
    memcpy(buf, it->second.data(), toCopy);
    return toCopy;
  }

  bool isKey(const char* key) { return _store.count(key) > 0; }
  size_t freeEntries() { return 100; }

 private:
  std::string _ns;
  std::map<std::string, std::string> _store;
};

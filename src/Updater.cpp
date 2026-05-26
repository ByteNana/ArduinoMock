#include "Updater.h"

bool UpdateClass::_beginResult = false;
size_t UpdateClass::_writeResult = 0;
uint8_t UpdateClass::_error = 0;
int UpdateClass::_endCallCount = 0;
bool UpdateClass::_lastEndArg = false;

bool UpdateClass::begin(size_t size, int command) {
  (void)size;
  (void)command;
  if (!_beginResult)
    _error = 1;
  return _beginResult;
}

bool UpdateClass::end(bool evenIfRemaining) {
  _endCallCount++;
  _lastEndArg = evenIfRemaining;
  return true;
}

size_t UpdateClass::writeStream(Stream& stream) {
  (void)stream;
  return _writeResult;
}

UpdateClass Update;

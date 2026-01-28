#ifndef ARDUINOMOCK_USE_GMOCK

#include "Stream.h"

void MockStream::SetupDefaults() {
  // Do nothing
}

void MockStream::InjectRxData(const std::string& data) {
  // Do nothing
}

std::string MockStream::GetTxData() { return ""; }

void MockStream::ClearTxData() {
  // Do nothing
}

void MockStream::ClearRxData() {
  // Do nothing
}

#endif

#include <gtest/gtest.h>

#include "Update.h"

class UpdaterTest : public ::testing::Test {
 protected:
  void SetUp() override { UpdateClass::reset(); }
};

// --- Defaults after reset ---

TEST_F(UpdaterTest, BeginDefaultReturnsFalse) { EXPECT_FALSE(Update.begin()); }

TEST_F(UpdaterTest, BeginWithSizeReturnsFalse) { EXPECT_FALSE(Update.begin(1024)); }

TEST_F(UpdaterTest, BeginWithCommandReturnsFalse) {
  EXPECT_FALSE(Update.begin(1024, U_FLASH));
  EXPECT_FALSE(Update.begin(1024, U_SPIFFS));
}

TEST_F(UpdaterTest, EndReturnsTrueByDefault) { EXPECT_TRUE(Update.end()); }

TEST_F(UpdaterTest, HasErrorReturnsFalse) { EXPECT_FALSE(Update.hasError()); }

TEST_F(UpdaterTest, ErrorStringIsEmpty) { EXPECT_STREQ(Update.errorString(), ""); }

TEST_F(UpdaterTest, IsRunningReturnsFalse) { EXPECT_FALSE(Update.isRunning()); }

TEST_F(UpdaterTest, IsFinishedReturnsFalse) { EXPECT_FALSE(Update.isFinished()); }

TEST_F(UpdaterTest, SizeReturnsZero) { EXPECT_EQ(Update.size(), 0u); }

TEST_F(UpdaterTest, ProgressReturnsZero) { EXPECT_EQ(Update.progress(), 0u); }

TEST_F(UpdaterTest, RemainingReturnsZero) { EXPECT_EQ(Update.remaining(), 0u); }

TEST_F(UpdaterTest, AbortDoesNotCrash) { Update.abort(); }

TEST_F(UpdaterTest, ClearErrorDoesNotCrash) { Update.clearError(); }

TEST_F(UpdaterTest, GlobalUpdateInstanceIsAccessible) {
  UpdateClass* ptr = &Update;
  EXPECT_NE(ptr, nullptr);
}

TEST_F(UpdaterTest, UFlashAndUSpiffsDefined) {
  EXPECT_EQ(U_FLASH, 0);
  EXPECT_EQ(U_SPIFFS, 100);
}

TEST_F(UpdaterTest, GetErrorReturnsZeroAfterReset) {
  EXPECT_EQ(Update.getError(), static_cast<uint8_t>(0));
}

// --- Test configuration helpers ---

TEST_F(UpdaterTest, SetBeginResultTrue) {
  UpdateClass::setBeginResult(true);
  EXPECT_TRUE(Update.begin());
}

TEST_F(UpdaterTest, SetBeginResultFalse) {
  UpdateClass::setBeginResult(false);
  EXPECT_FALSE(Update.begin());
}

TEST_F(UpdaterTest, SetWriteStreamResult) {
  UpdateClass::setWriteStreamResult(512);
  // writeStream needs a Stream& — use a minimal stand-in via a null cast
  // The stub ignores the stream object entirely
  Stream* s = nullptr;
  EXPECT_EQ(Update.writeStream(*s), 512u);
}

TEST_F(UpdaterTest, SetErrorConfiguresGetError) {
  UpdateClass::setError(7);
  EXPECT_EQ(Update.getError(), static_cast<uint8_t>(7));
}

TEST_F(UpdaterTest, ClearErrorResetsToZero) {
  UpdateClass::setError(3);
  Update.clearError();
  EXPECT_EQ(Update.getError(), static_cast<uint8_t>(0));
}

// --- Observability ---

TEST_F(UpdaterTest, EndCallCountStartsAtZero) { EXPECT_EQ(UpdateClass::endCallCount(), 0); }

TEST_F(UpdaterTest, EndCallCountIncrementsOnEachCall) {
  Update.end();
  Update.end();
  EXPECT_EQ(UpdateClass::endCallCount(), 2);
}

TEST_F(UpdaterTest, LastEndArgReflectsCommitTrue) {
  Update.end(true);
  EXPECT_TRUE(UpdateClass::lastEndArg());
}

TEST_F(UpdaterTest, LastEndArgReflectsCommitFalse) {
  Update.end(false);
  EXPECT_FALSE(UpdateClass::lastEndArg());
}

TEST_F(UpdaterTest, ResetClearsEndCallCount) {
  Update.end();
  UpdateClass::reset();
  EXPECT_EQ(UpdateClass::endCallCount(), 0);
}

// --- OTA scenario tests ---

TEST_F(UpdaterTest, SuccessfulOTA) {
  UpdateClass::setBeginResult(true);
  UpdateClass::setWriteStreamResult(1024);

  ASSERT_TRUE(Update.begin(1024));
  Stream* s = nullptr;
  EXPECT_EQ(Update.writeStream(*s), 1024u);
  Update.end(true);

  EXPECT_EQ(UpdateClass::endCallCount(), 1);
  EXPECT_TRUE(UpdateClass::lastEndArg());
}

TEST_F(UpdaterTest, PartialWriteAborts) {
  UpdateClass::setBeginResult(true);
  UpdateClass::setWriteStreamResult(512);  // only half written

  ASSERT_TRUE(Update.begin(1024));
  Stream* s = nullptr;
  size_t written = Update.writeStream(*s);
  bool commit = (written == 1024);
  Update.end(commit);

  EXPECT_EQ(UpdateClass::endCallCount(), 1);
  EXPECT_FALSE(UpdateClass::lastEndArg());  // abort — partition protected
}

TEST_F(UpdaterTest, BeginFailureNeverCallsEnd) {
  UpdateClass::setBeginResult(false);

  if (Update.begin(1024)) {
    Update.end(true);
  }

  EXPECT_EQ(UpdateClass::endCallCount(), 0);
}

TEST_F(UpdaterTest, BeginFailureSetsError) {
  UpdateClass::setBeginResult(false);
  Update.begin(1024);
  EXPECT_NE(Update.getError(), static_cast<uint8_t>(0));
}

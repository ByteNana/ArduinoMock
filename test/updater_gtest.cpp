#include <gtest/gtest.h>

#include "Update.h"

TEST(UpdaterTest, BeginDefaultReturnsFalse) { EXPECT_FALSE(Update.begin()); }

TEST(UpdaterTest, BeginWithSizeReturnsFalse) { EXPECT_FALSE(Update.begin(1024)); }

TEST(UpdaterTest, EndReturnsFalse) { EXPECT_FALSE(Update.end()); }

TEST(UpdaterTest, HasErrorReturnsFalse) { EXPECT_FALSE(Update.hasError()); }

TEST(UpdaterTest, ErrorStringIsEmpty) { EXPECT_STREQ(Update.errorString(), ""); }

TEST(UpdaterTest, IsRunningReturnsFalse) { EXPECT_FALSE(Update.isRunning()); }

TEST(UpdaterTest, IsFinishedReturnsFalse) { EXPECT_FALSE(Update.isFinished()); }

TEST(UpdaterTest, SizeReturnsZero) { EXPECT_EQ(Update.size(), 0u); }

TEST(UpdaterTest, ProgressReturnsZero) { EXPECT_EQ(Update.progress(), 0u); }

TEST(UpdaterTest, RemainingReturnsZero) { EXPECT_EQ(Update.remaining(), 0u); }

TEST(UpdaterTest, AbortDoesNotCrash) { Update.abort(); }

TEST(UpdaterTest, ClearErrorDoesNotCrash) { Update.clearError(); }

TEST(UpdaterTest, GlobalUpdateInstanceIsAccessible) {
  UpdateClass* ptr = &Update;
  EXPECT_NE(ptr, nullptr);
}

TEST(UpdaterTest, UFlashAndUSpiffsDefined) {
  EXPECT_EQ(U_FLASH, 0);
  EXPECT_EQ(U_SPIFFS, 100);
}

TEST(UpdaterTest, BeginWithCommand) {
  EXPECT_FALSE(Update.begin(1024, U_FLASH));
  EXPECT_FALSE(Update.begin(1024, U_SPIFFS));
}

TEST(UpdaterTest, GetErrorReturnsZero) { EXPECT_EQ(Update.getError(), static_cast<uint8_t>(0)); }

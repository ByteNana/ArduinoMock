#include <gtest/gtest.h>

#include "LittleFS.h"
#include "SPIFFS.h"

class SPIFFSTest : public ::testing::Test {
 protected:
  void SetUp() override { SPIFFS.reset(); }
};

TEST_F(SPIFFSTest, BeginReturnsTrueByDefault) { EXPECT_TRUE(SPIFFS.begin()); }

TEST_F(SPIFFSTest, BeginRespectsSetMounted) {
  SPIFFS.setMounted(false);
  EXPECT_FALSE(SPIFFS.begin());
}

TEST_F(SPIFFSTest, ReadPrePopulatedFile) {
  SPIFFS.addFile("/config.json", "{\"key\":\"value\"}");
  File f = SPIFFS.open("/config.json", FILE_READ);
  EXPECT_TRUE(static_cast<bool>(f));
  EXPECT_STREQ(f.readString().c_str(), "{\"key\":\"value\"}");
}

TEST_F(SPIFFSTest, WriteAndReadBack) {
  File f = SPIFFS.open("/test.txt", FILE_WRITE);
  EXPECT_TRUE(static_cast<bool>(f));
  f.print("hello");
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/test.txt").c_str(), "hello");
}

TEST_F(SPIFFSTest, AppendPreservesContent) {
  SPIFFS.addFile("/log.txt", "line1\n");
  File f = SPIFFS.open("/log.txt", FILE_APPEND);
  f.print("line2\n");
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/log.txt").c_str(), "line1\nline2\n");
}

TEST_F(SPIFFSTest, WriteModeClears) {
  SPIFFS.addFile("/data.txt", "old content");
  File f = SPIFFS.open("/data.txt", FILE_WRITE);
  f.print("new");
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/data.txt").c_str(), "new");
}

TEST_F(SPIFFSTest, OpenNonExistentReadReturnsFalsy) {
  File f = SPIFFS.open("/nope.txt", FILE_READ);
  EXPECT_FALSE(static_cast<bool>(f));
}

TEST_F(SPIFFSTest, ExistsAndRemove) {
  SPIFFS.addFile("/tmp.txt", "data");
  EXPECT_TRUE(SPIFFS.exists("/tmp.txt"));
  EXPECT_TRUE(SPIFFS.remove("/tmp.txt"));
  EXPECT_FALSE(SPIFFS.exists("/tmp.txt"));
}

TEST_F(SPIFFSTest, Rename) {
  SPIFFS.addFile("/old.txt", "content");
  EXPECT_TRUE(SPIFFS.rename("/old.txt", "/new.txt"));
  EXPECT_FALSE(SPIFFS.exists("/old.txt"));
  EXPECT_TRUE(SPIFFS.exists("/new.txt"));
  EXPECT_STREQ(SPIFFS.getFile("/new.txt").c_str(), "content");
}

TEST_F(SPIFFSTest, ByteByByteReadAndPeek) {
  SPIFFS.addFile("/abc.txt", "ABC");
  File f = SPIFFS.open("/abc.txt", FILE_READ);
  EXPECT_EQ(f.available(), 3);
  EXPECT_EQ(f.peek(), 'A');
  EXPECT_EQ(f.read(), 'A');
  EXPECT_EQ(f.available(), 2);
  EXPECT_EQ(f.read(), 'B');
  EXPECT_EQ(f.read(), 'C');
  EXPECT_EQ(f.read(), -1);
  EXPECT_EQ(f.available(), 0);
}

TEST_F(SPIFFSTest, PrintAndPrintln) {
  File f = SPIFFS.open("/out.txt", FILE_WRITE);
  f.println("hello");
  f.print("world");
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/out.txt").c_str(), "hello\nworld");
}

TEST_F(SPIFFSTest, FileNameAndSize) {
  SPIFFS.addFile("/info.txt", "12345");
  File f = SPIFFS.open("/info.txt", FILE_READ);
  EXPECT_STREQ(f.name().c_str(), "/info.txt");
  EXPECT_EQ(f.size(), 5u);
}

TEST_F(SPIFFSTest, WriteThroughToBackingStore) {
  File f = SPIFFS.open("/live.txt", FILE_WRITE);
  const uint8_t data[] = {'H', 'i'};
  f.write(data, 2);
  // Verify backing store updated BEFORE close
  EXPECT_STREQ(SPIFFS.getFile("/live.txt").c_str(), "Hi");
  f.close();
}

TEST_F(SPIFFSTest, WriteUint8) {
  File f = SPIFFS.open("/byte.txt", FILE_WRITE);
  f.write(static_cast<uint8_t>('X'));
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/byte.txt").c_str(), "X");
}

TEST_F(SPIFFSTest, ResetClearsEverything) {
  SPIFFS.addFile("/a.txt", "data");
  SPIFFS.setMounted(false);
  SPIFFS.reset();
  EXPECT_TRUE(SPIFFS.begin());
  EXPECT_FALSE(SPIFFS.exists("/a.txt"));
}

TEST_F(SPIFFSTest, LittleFSAlias) {
  LittleFS.reset();
  LittleFS.addFile("/fs.txt", "littlefs");
  File f = LittleFS.open("/fs.txt", FILE_READ);
  EXPECT_TRUE(static_cast<bool>(f));
  EXPECT_STREQ(f.readString().c_str(), "littlefs");
}

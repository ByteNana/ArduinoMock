#include <gtest/gtest.h>

#include "Arduino.h"
#include "FS.h"
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

TEST_F(SPIFFSTest, ReadStringUntilStopsAtDelimiter) {
  SPIFFS.addFile("/delim.txt", "hello|world");
  File f = SPIFFS.open("/delim.txt", FILE_READ);
  String result = f.readStringUntil('|');
  EXPECT_STREQ(result.c_str(), "hello");
}

TEST_F(SPIFFSTest, ReadStringUntilNewlineReturnsFirstLine) {
  SPIFFS.addFile("/lines.txt", "line1\nline2\nline3");
  File f = SPIFFS.open("/lines.txt", FILE_READ);
  String result = f.readStringUntil('\n');
  EXPECT_STREQ(result.c_str(), "line1");
}

TEST_F(SPIFFSTest, ReadStringUntilAtEofReturnsRemaining) {
  SPIFFS.addFile("/eof.txt", "abc");
  File f = SPIFFS.open("/eof.txt", FILE_READ);
  // Read past first byte so we're mid-stream, then read until '\n' (not present)
  f.read();  // consume 'a'
  String result = f.readStringUntil('\n');
  EXPECT_STREQ(result.c_str(), "bc");
}

TEST_F(SPIFFSTest, ReadStringUntilDelimiterNotPresentReturnsFullContent) {
  SPIFFS.addFile("/full.txt", "no delimiter here");
  File f = SPIFFS.open("/full.txt", FILE_READ);
  String result = f.readStringUntil('|');
  EXPECT_STREQ(result.c_str(), "no delimiter here");
}

TEST_F(SPIFFSTest, FSHeaderIncludeCompiles) {
  // This test verifies that including FS.h (which re-exports SPIFFS.h) compiles
  // and the File/MockSPIFFS types are accessible via that include path.
  SPIFFS.addFile("/fstest.txt", "via FS.h");
  File f = SPIFFS.open("/fstest.txt", FILE_READ);
  EXPECT_TRUE(static_cast<bool>(f));
  EXPECT_STREQ(f.readString().c_str(), "via FS.h");
}

TEST_F(SPIFFSTest, LittleFSBeginFormatOnFailReturnsTrue) {
  LittleFS.reset();
  EXPECT_TRUE(LittleFS.begin(true));
}

// --- end() ---

TEST_F(SPIFFSTest, EndAllowsRemounting) {
  EXPECT_TRUE(SPIFFS.begin());
  SPIFFS.end();
  EXPECT_TRUE(SPIFFS.begin());  // end() doesn't prevent remounting
}

// --- begin(bool, const char*, uint8_t) ---

TEST_F(SPIFFSTest, ThreeArgBeginReturnsTrue) { EXPECT_TRUE(SPIFFS.begin(false, "/spiffs", 10)); }

TEST_F(SPIFFSTest, ThreeArgBeginRespectsMountedState) {
  SPIFFS.setMounted(false);
  EXPECT_FALSE(SPIFFS.begin(false, "/spiffs", 10));
}

// --- open(String) ---

TEST_F(SPIFFSTest, OpenWithStringPath) {
  SPIFFS.addFile("/cfg.json", "{}");
  File f = SPIFFS.open(String("/cfg.json"), FILE_READ);
  EXPECT_TRUE(static_cast<bool>(f));
  EXPECT_STREQ(f.readString().c_str(), "{}");
}

TEST_F(SPIFFSTest, OpenWithStringPathWriteMode) {
  File f = SPIFFS.open(String("/new.txt"), FILE_WRITE);
  EXPECT_TRUE(static_cast<bool>(f));
  f.print("data");
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/new.txt").c_str(), "data");
}

// --- exists(String) ---

TEST_F(SPIFFSTest, ExistsWithStringPath) {
  SPIFFS.addFile("/x.txt", "");
  EXPECT_TRUE(SPIFFS.exists(String("/x.txt")));
  EXPECT_FALSE(SPIFFS.exists(String("/y.txt")));
}

// --- remove(String) ---

TEST_F(SPIFFSTest, RemoveWithStringPath) {
  SPIFFS.addFile("/del.txt", "bye");
  EXPECT_TRUE(SPIFFS.remove(String("/del.txt")));
  EXPECT_FALSE(SPIFFS.exists("/del.txt"));
}

// --- File::print(String) / println(String) ---

TEST_F(SPIFFSTest, FilePrintString) {
  File f = SPIFFS.open("/s.txt", FILE_WRITE);
  f.print(String("hello"));
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/s.txt").c_str(), "hello");
}

TEST_F(SPIFFSTest, FilePrintlnString) {
  File f = SPIFFS.open("/sl.txt", FILE_WRITE);
  f.println(String("hello"));
  f.close();
  EXPECT_STREQ(SPIFFS.getFile("/sl.txt").c_str(), "hello\n");
}

// --- File::openNextFile() ---

TEST_F(SPIFFSTest, OpenNextFileReturnsInvalidFile) {
  SPIFFS.addFile("/dir/", "");
  File dir = SPIFFS.open("/dir/", FILE_READ);
  File entry = dir.openNextFile();
  EXPECT_FALSE(static_cast<bool>(entry));
}

// --- F() / PSTR() ---

TEST_F(SPIFFSTest, PSTRMacroIsIdentity) {
  const char* s = PSTR("hello");
  EXPECT_STREQ(s, "hello");
}

TEST_F(SPIFFSTest, FMacroCompilesAndPassesThrough) {
  const __FlashStringHelper* fstr = F("world");
  EXPECT_STREQ(reinterpret_cast<const char*>(fstr), "world");
}

// --- File::read(uint8_t*, size_t) ---

TEST_F(SPIFFSTest, BulkReadReadsExactBytes) {
  SPIFFS.addFile("/bin.dat", "ABCDE");
  File f = SPIFFS.open("/bin.dat", FILE_READ);
  uint8_t buf[3] = {};
  EXPECT_EQ(f.read(buf, 3), 3u);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(buf[2], 'C');
  EXPECT_EQ(f.available(), 2);
}

TEST_F(SPIFFSTest, BulkReadAdvancesPosition) {
  SPIFFS.addFile("/seq.dat", "12345");
  File f = SPIFFS.open("/seq.dat", FILE_READ);
  uint8_t first[2] = {};
  uint8_t second[2] = {};
  f.read(first, 2);
  f.read(second, 2);
  EXPECT_EQ(first[0], '1');
  EXPECT_EQ(first[1], '2');
  EXPECT_EQ(second[0], '3');
  EXPECT_EQ(second[1], '4');
}

TEST_F(SPIFFSTest, BulkReadClampsAtEof) {
  SPIFFS.addFile("/short.dat", "AB");
  File f = SPIFFS.open("/short.dat", FILE_READ);
  uint8_t buf[10] = {};
  EXPECT_EQ(f.read(buf, 10), 2u);
  EXPECT_EQ(buf[0], 'A');
  EXPECT_EQ(buf[1], 'B');
  EXPECT_EQ(f.available(), 0);
}

TEST_F(SPIFFSTest, BulkReadAtEofReturnsZero) {
  SPIFFS.addFile("/empty.dat", "");
  File f = SPIFFS.open("/empty.dat", FILE_READ);
  uint8_t buf[4] = {};
  EXPECT_EQ(f.read(buf, 4), 0u);
}

TEST_F(SPIFFSTest, BulkReadNullBufReturnsZero) {
  SPIFFS.addFile("/null.dat", "data");
  File f = SPIFFS.open("/null.dat", FILE_READ);
  EXPECT_EQ(f.read(nullptr, 4), 0u);
}

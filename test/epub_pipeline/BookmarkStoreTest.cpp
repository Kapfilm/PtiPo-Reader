#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "src/BookmarkStore.h"

namespace {
class BookmarkStoreTest : public testing::Test {
 protected:
  std::filesystem::path work;
  void SetUp() override {
    work = std::filesystem::temp_directory_path() /
           (std::string("paperio_bookmark_") + testing::UnitTest::GetInstance()->current_test_info()->name());
    std::filesystem::remove_all(work);
    std::filesystem::create_directories(work);
  }
  void TearDown() override { std::filesystem::remove_all(work); }
  void legacy(uint8_t version, uint16_t anchorLength = 0) {
    std::ofstream f(work / "bookmarks.bin", std::ios::binary);
    auto pod = [&f](auto value) { f.write(reinterpret_cast<const char*>(&value), sizeof(value)); };
    pod(version);
    pod(uint16_t{1});
    pod(uint16_t{7});
    pod(uint16_t{2});
    if (version >= 2) {
      pod(uint16_t{4});
      f.write("Name", 4);
    }
    if (version >= 3) pod(anchorLength);
  }
};

TEST_F(BookmarkStoreTest, SeparatesNormalPreviewAndFullNoteCoordinatesAfterReload) {
  BookmarkStore store;
  store.load(work.string());
  ASSERT_TRUE(store.toggle(7, 2));
  ASSERT_TRUE(store.toggle(7, 2, "verse-one"));
  ASSERT_TRUE(store.toggle(7, 2, "verse-two"));
  ASSERT_TRUE(store.toggle(7, 2, {}, true));
  store.rename(1, "Verse bookmark");
  store.save();
  BookmarkStore reloaded;
  reloaded.load(work.string());
  ASSERT_EQ(reloaded.getAll().size(), 4);
  EXPECT_TRUE(reloaded.has(7, 2));
  EXPECT_TRUE(reloaded.has(7, 2, "verse-one"));
  EXPECT_TRUE(reloaded.has(7, 2, "verse-two"));
  EXPECT_TRUE(reloaded.has(7, 2, {}, true));
  EXPECT_EQ(reloaded.getAll()[1].name, "Verse bookmark");
  EXPECT_FALSE(reloaded.toggle(7, 2, "verse-one"));
  EXPECT_TRUE(reloaded.has(7, 2));
  EXPECT_TRUE(reloaded.has(7, 2, "verse-two"));
}

TEST_F(BookmarkStoreTest, ReadsBothStableFormatsWithoutNoteContext) {
  for (uint8_t version : {1, 2}) {
    legacy(version);
    BookmarkStore store;
    store.load(work.string());
    ASSERT_EQ(store.getAll().size(), 1);
    EXPECT_TRUE(store.has(7, 2));
    EXPECT_FALSE(store.has(7, 2, {}, true));
    EXPECT_TRUE(store.getAll()[0].previewAnchor.empty());
    EXPECT_EQ(store.getAll()[0].name, version == 2 ? "Name" : "");
  }
}

TEST_F(BookmarkStoreTest, RejectsOversizedAndTruncatedPreviewAnchors) {
  for (uint16_t length : {192, 10}) {
    legacy(3, length);
    BookmarkStore store;
    store.load(work.string());
    EXPECT_TRUE(store.isEmpty());
  }
  BookmarkStore store;
  store.load(work.string());
  EXPECT_FALSE(store.toggle(7, 2, std::string(192, 'x')));
  EXPECT_TRUE(store.isEmpty());
  EXPECT_TRUE(store.toggle(7, 2, std::string(191, 'x')));
  store.save();
  BookmarkStore reloaded;
  reloaded.load(work.string());
  EXPECT_TRUE(reloaded.has(7, 2, std::string(191, 'x')));
}
TEST_F(BookmarkStoreTest, ReportsWriteFailureAndCanRetryWithoutLosingNoteBookmark) {
  const auto missing = work / "blocked-by-file";
  {
    std::ofstream blocker(missing);
    blocker << "not a directory";
  }
  BookmarkStore store;
  store.load(missing.string());
  ASSERT_TRUE(store.toggle(7, 2, "ps112v05"));
  EXPECT_FALSE(store.save());
  std::filesystem::remove(missing);
  std::filesystem::create_directories(missing);
  ASSERT_TRUE(store.save());
  BookmarkStore reloaded;
  reloaded.load(missing.string());
  EXPECT_TRUE(reloaded.has(7, 2, "ps112v05"));
  EXPECT_FALSE(reloaded.has(7, 2));
}

TEST_F(BookmarkStoreTest, RemovingNoteBookmarkIsPersistedWithoutRemovingOtherContexts) {
  BookmarkStore store;
  store.load(work.string());
  ASSERT_TRUE(store.toggle(7, 2));
  ASSERT_TRUE(store.toggle(7, 2, "ps112v05"));
  ASSERT_TRUE(store.save());
  EXPECT_FALSE(store.toggle(7, 2, "ps112v05"));
  ASSERT_TRUE(store.save());
  BookmarkStore reloaded;
  reloaded.load(work.string());
  EXPECT_TRUE(reloaded.has(7, 2));
  EXPECT_FALSE(reloaded.has(7, 2, "ps112v05"));
}

}  // namespace

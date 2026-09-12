#include <GlobalBookmarkIndex.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>

class GlobalBookmarkIndexTest : public ::testing::Test {
 protected:
  void SetUp() override {
    char root[] = "/tmp/paperio-global-bookmarks-XXXXXX";
    ASSERT_NE(mkdtemp(root), nullptr);
    Storage.root = root;
    GLOBAL_BOOKMARKS.load();
  }
  void TearDown() override { std::filesystem::remove_all(Storage.root); }
};

TEST_F(GlobalBookmarkIndexTest, RoundTripsNoteBookmarksFromBookStore) {
  Storage.mkdir("/cache");
  BookmarkStore store;
  store.load("/cache");
  ASSERT_TRUE(store.toggle(5, 1));
  ASSERT_TRUE(store.toggle(5, 1, "ps112v05"));
  ASSERT_TRUE(store.toggle(5, 1, "ps112v05", true));
  store.save();
  GLOBAL_BOOKMARKS.syncFromStore(store, "/bible.epub", "/cache", "Bible", false);
  GLOBAL_BOOKMARKS.load();
  ASSERT_EQ(GLOBAL_BOOKMARKS.getEntries().size(), 1u);
  const auto& saved = GLOBAL_BOOKMARKS.getEntries().front().bookmarks;
  ASSERT_EQ(saved.size(), 3u);
  EXPECT_TRUE(saved[0].previewAnchor.empty());
  EXPECT_EQ(saved[1].previewAnchor, "ps112v05");
  EXPECT_FALSE(saved[1].fullNote);
  EXPECT_EQ(saved[2].previewAnchor, "ps112v05");
  EXPECT_TRUE(saved[2].fullNote);
}

TEST_F(GlobalBookmarkIndexTest, ReadsStableIndexWithoutAddingNoteContext) {
  Storage.mkdir("/.crosspoint");
  std::ofstream file(Storage.path("/.crosspoint/global_bookmarks.bin"), std::ios::binary);
  const auto pod = [&](auto value) { file.write(reinterpret_cast<const char*>(&value), sizeof(value)); };
  const auto text = [&](const std::string& value) {
    pod(static_cast<uint16_t>(value.size()));
    file.write(value.data(), value.size());
  };
  pod(uint8_t{1});
  pod(uint16_t{1});
  text("/book.epub");
  text("/cache");
  text("Book");
  pod(uint8_t{0});
  pod(uint16_t{1});
  pod(uint16_t{5});
  pod(uint16_t{1});
  text("Old bookmark");
  file.close();
  GLOBAL_BOOKMARKS.load();
  ASSERT_EQ(GLOBAL_BOOKMARKS.getEntries().size(), 1u);
  const auto& saved = GLOBAL_BOOKMARKS.getEntries().front().bookmarks;
  ASSERT_EQ(saved.size(), 1u);
  EXPECT_EQ(saved[0].name, "Old bookmark");
  EXPECT_TRUE(saved[0].previewAnchor.empty());
  EXPECT_FALSE(saved[0].fullNote);
}

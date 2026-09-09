#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "Epub.h"
#include "Epub/FootnotePreviews.h"
#include "src/util/FootnoteHistory.h"

namespace {
class FootnoteNavigationTest : public testing::Test {
 protected:
  std::filesystem::path work;
  void SetUp() override {
    work = std::filesystem::temp_directory_path() /
           (std::string("paperio_note_") + testing::UnitTest::GetInstance()->current_test_info()->name());
    std::filesystem::remove_all(work);
    std::filesystem::create_directories(work);
  }
  void TearDown() override { std::filesystem::remove_all(work); }
};

TEST_F(FootnoteNavigationTest, ResolvesPathsRelativeToSourceBeforeBasenames) {
  Epub book(CORPUS_DIR "/../fixtures/footnote_paths.epub", work.string());
  ASSERT_TRUE(book.load(true));
  EXPECT_EQ(book.resolveHrefToSpineIndex("../B/notes.xhtml#n", 0), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("notes.xhtml#n", 1), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("OPS/B/notes.xhtml#n"), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("B/notes.xhtml#n"), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("/OPS/B/notes.xhtml#n", 0), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("#n", 1), 1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("../B/extra%20notes.xhtml#note%20two", 0), 2);
  EXPECT_EQ(book.resolveHrefToSpineIndex("notes.xhtml#n"), -1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("absent/notes.xhtml#n", 0), -1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("https://example.com/notes.xhtml#n", 0), -1);
  EXPECT_EQ(book.resolveHrefToSpineIndex("#n", 999), -1);
}

TEST_F(FootnoteNavigationTest, PreviewUsesCorrectFileAndDecodedAnchor) {
  Epub book(CORPUS_DIR "/../fixtures/footnote_paths.epub", work.string());
  ASSERT_TRUE(book.load(true));
  ASSERT_TRUE(FootnotePreviews::gather(book));
  ASSERT_TRUE(FootnotePreviews::cacheExists(book.getCachePath()));
  FootnotePreviews::Lookup lookup;
  ASSERT_TRUE(lookup.open(book.getCachePath(), &book, 0));
  std::string text;
  ASSERT_TRUE(lookup.find("../B/notes.xhtml#n", text));
  EXPECT_EQ(text, "CORRECT_NOTE_FROM_B");
  ASSERT_TRUE(lookup.find("../B/extra%20notes.xhtml#note%20two", text));
  EXPECT_EQ(text, "ESCAPED_NOTE_TEXT");
}

TEST_F(FootnoteNavigationTest, OldAndTruncatedPreviewCachesAreNotAccepted) {
  Epub book(CORPUS_DIR "/../fixtures/footnote_paths.epub", work.string());
  ASSERT_TRUE(book.load(true));
  ASSERT_TRUE(FootnotePreviews::gather(book));
  const auto path = book.getCachePath() + FootnotePreviews::CACHE_FILENAME;
  {
    std::fstream file(path, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(4);
    const char oldVersion[2] = {1, 0};
    file.write(oldVersion, 2);
  }
  EXPECT_FALSE(FootnotePreviews::cacheExists(book.getCachePath()));
  ASSERT_TRUE(FootnotePreviews::gather(book));
  std::filesystem::resize_file(path, 7);
  EXPECT_FALSE(FootnotePreviews::cacheExists(book.getCachePath()));
}

TEST(FootnoteHistoryTest, NestedPreviewReturnsToItsOwnCoordinatesAndPreservesRoot) {
  FootnoteHistory history;
  EXPECT_FALSE(history.pop().has_value());
  EXPECT_TRUE(history.push({2, 47, 100, 321, "", ""}));
  EXPECT_TRUE(history.push({9, 1, 3, 8, "note-seven", "note-seven"}));
  EXPECT_EQ(history.root().pageNumber, 47);
  auto note = history.pop();
  ASSERT_TRUE(note);
  EXPECT_EQ(note->previewAnchor, "note-seven");
  EXPECT_EQ(note->pageNumber, 1);
  EXPECT_FALSE(history.empty());
  auto root = history.pop();
  EXPECT_EQ(root->spineIndex, 2);
  EXPECT_EQ(root->pageNumber, 47);
  EXPECT_EQ(root->pageCount, 100);
  EXPECT_TRUE(history.empty());
}

TEST(FootnoteHistoryTest, FullHistoryRejectsNewJumpWithoutLosingReturnPositions) {
  FootnoteHistory history;
  for (int i = 0; i < FootnoteHistory::CAPACITY; ++i) ASSERT_TRUE(history.push({i, i + 10}));
  ASSERT_TRUE(history.full());
  EXPECT_FALSE(history.push({999, 999}));
  EXPECT_EQ(history.size(), FootnoteHistory::CAPACITY);
  EXPECT_EQ(history.pop()->spineIndex, FootnoteHistory::CAPACITY - 1);
  history.keepRoot();
  EXPECT_EQ(history.size(), 1);
  EXPECT_EQ(history.pop()->pageNumber, 10);
}
}  // namespace

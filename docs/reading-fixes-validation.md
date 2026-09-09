# Paperio 1.1.0-reading-fixes.1

Based on the Paperio-Reader branch at 258b6984, not the stale upstream 1.1.0 tag.
This is a local test build; no GitHub release or device upload is performed.

## Changes

- EPUB link resolution uses the source document directory, then exact archive/OPF paths. Ambiguous basename fallback is rejected.
- URI escapes in note fragments are decoded consistently when gathering and looking up previews.
- Footnote preview cache version 2 rejects old/truncated caches; inline layout revision 3 rebuilds pages with corrected previews. Main reading progress is retained.
- Note navigation commits the original reading position before leaving it. All normal progress writes are suppressed while a note history exists; KOReader sync uses the source position/paragraph.
- History retains preview anchors and page coordinates for eight nested references; a ninth is rejected without discarding history.
- The note window uses effective book typography and orientation-aware hint gutters. Confirm opens full-text/linked-notes/previous-next-note/return actions. Back restores one level; Return to book restores the root. The preview remains bounded to three pages; full text opens the source chapter at the note anchor.
- Notes never page into adjacent chapters. The note list is shared by the reader menu and assigned button, including cached descriptions.
- TOC side-button taps move five entries, clamped at either end. Front navigation remains one entry. Only one alias per physical side button is consumed.
- Per-book typography presets: Medium/100%, Extra large/130%, Small/90%, or global defaults. Other overrides are preserved.

## Automated checks

`EpubPipelineTest`: all 55 tests passed (five new navigation/history regressions).
`PreviewBlockLocatorTest`: all eight tests passed.
The eight updated goldens change image cache filenames only, due to the layout revision; geometry and text are unchanged.

Configure host tests with CMake, then build the two targets and execute them.
Full aggregate host builds also include a Linux-specific malloc benchmark and are not required on macOS.
Firmware: `pio run -e gh_release`, Arduino ESP32 3.3.7 as required by the platform config.
Final build passed on 2026-09-09: static RAM 55,140/327,680 bytes (16.8%),
flash 4,649,671/6,553,600 bytes (70.9%). These are build measurements, not runtime heap measurements.
The delivery image passes esptool checksum and SHA-256 validation and includes the
`1.1.0-reading-fixes.1` version string. The repository's release header patch was applied.

## Device verification still required

Copy `test/fixtures/reading_navigation.epub` to the SD card:

1. Open chapter 1, remember its page, and open reference 1.
2. Page forward to the preview end; choose Read full text. Reach the end marker after paragraph 80. Back must return to the original book page.
3. Open reference 2 in the same chapter. Sleep or power off inside the note; reopen the book and verify the source page is preserved.
4. Open reference 3, follow reference 4, then press Back twice. Verify the intermediate note and source page.
5. Repeat a next-page turn after waiting for pre-render to complete; sleep inside the note and verify source restoration.
6. Check previous/next note in the note menu. Reopen the note list and verify selection is retained.
7. In the 60-chapter TOC, tap the side buttons: 1 -> 6 -> 11 and back. At either edge selection must stop; front buttons must still move one entry.
8. Repeat note navigation and the TOC in portrait, inverted portrait and both landscape orientations; ensure hints do not overlap text.
9. Choose each reading profile, close/reopen the book and verify size/spacing persist. A different book must retain its own settings.
10. With KOReader auto-push configured, close while in a note and check that the main-book position is sent, not a preview-local page.

Memory: one source reference list (at most 32 entries) and eight bounded history entries are retained during note reading. No whole-note text buffer or second rendering engine is added. Check runtime free/contiguous heap and E-Ink refresh quality on X4/X3 hardware.

# Interface typography — 1.1.0-reading-fixes.3

Settings → Display → Interface font offers Default, Dark and Golos Text. Default retains the original Inter 10/12 and Noto Sans 8 families. Dark uses the existing Inter Bold faces and a new auto-hinted, 1-bit Noto Sans Bold 8 face generated from the repository's font source. The choice is persisted by the standard JSON settings schema, which also rejects invalid enum values.

The render task applies family replacement while holding RenderLock, before drawing an activity. The existing font-map nodes are updated without allocation. Measurement, truncation, rotated text and drawing resolve through the same family. Scaled-glyph masks are invalidated when the choice changes. Reading font IDs are not replaced.

The additional font bitmap and metrics live in flash; there is one additional static EpdFont object. No framebuffer, image scaling or repeated text overpainting is added. This does not establish a measured runtime memory or speed result.

Validation:
- Firmware compilation and esptool image validation: see the delivery manifest.
- clang-format and git diff whitespace checks passed.
- A bitmap proof built from the generated headers verifies representative Russian glyphs, including Ё/Й, digits and button-label widths in both styles. The proof is a typography sample, not a device screenshot.

Hardware checks:
1. Select Dark in Settings → Display → Interface font. Check titles, values, selected rows and front/side button hints in the installed theme.
2. Close settings, open the library and a book menu; then reboot and verify the choice persists.
3. Select Default and check that the original typography returns.
4. Check long Russian labels, each UI theme and all four orientations for clipping or overlap.
5. Inspect thin details and inverted labels on E-Ink after a full refresh.

Earlier footnote navigation, TOC five-entry movement and per-book reading profiles are included unchanged.

## Golos Text option

Golos Text Medium (500) is used at sizes 8, 10 and 12; explicit bold UI text uses SemiBold (600) at sizes 10 and 12. Source and OFL license are stored under `lib/EpdFont/builtinFonts/source/GolosText`, pinned to googlefonts/golos-text commit d321a70cd12f84603617d4cf5d0e0321c2291dd9. `scripts/generate_golos_ui.py` generates the five headers with fontTools instancing and the existing font converter. Inter/Noto fallback retains the previous language coverage.

Rendering remains auto-hinted, 1-bit black/white; no grayscale antialiasing is enabled in this build. Five additional static font objects and flash-resident glyph data are added. The persisted enum appends value 2, preserving existing Default/Dark choices. Check all three choices and persistence after reboot on hardware.

Golos generation checks passed: previous character coverage is retained in each of the five faces (1,085 glyphs at size 8 and 1,036 at sizes 10/12). Bitmap offset/length bounds passed for all glyphs; representative Russian button labels fit their 106-pixel slots.

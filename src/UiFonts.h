#pragma once
#include <cstdint>

class GfxRenderer;

void registerUiFonts(GfxRenderer& renderer);
// Called under the render lock, only when the interface preference changes.
void applyUiFontStyle(GfxRenderer& renderer, uint8_t style);

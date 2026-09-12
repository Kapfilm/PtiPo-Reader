#pragma once

#include "../MenuListActivity.h"

// A small action menu; note text remains paginated by the normal EPUB engine.
class FootnoteMenuActivity final : public MenuListActivity {
 public:
  enum Action { HIGHLIGHT, DICTIONARY, SELECT_DICTIONARY, FULL_TEXT, LINKS, PREVIOUS_NOTE, NEXT_NOTE, RETURN_TO_BOOK };
  FootnoteMenuActivity(GfxRenderer& renderer, MappedInputManager& input, bool preview, bool links, bool siblings);
  void render(RenderLock&&) override;

 private:
  void onActionSelected(int index) override;
};

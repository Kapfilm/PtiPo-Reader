#include "FootnoteMenuActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "components/UITheme.h"
#include "fontIds.h"

FootnoteMenuActivity::FootnoteMenuActivity(GfxRenderer& renderer, MappedInputManager& input, bool preview, bool links,
                                           bool siblings)
    : MenuListActivity("FootnoteMenu", renderer, input) {
  // Bounded action descriptors; no note text is copied here.
  menuItems.reserve(8);
  menuItems.push_back(SettingInfo::Action(StrId::STR_CREATE_CLIPPING, SettingAction::None));
  menuItems.push_back(SettingInfo::Action(StrId::STR_LOOKUP, SettingAction::None));
  menuItems.push_back(SettingInfo::Action(StrId::STR_SELECT_DICTIONARY, SettingAction::None));
  if (preview) menuItems.push_back(SettingInfo::Action(StrId::STR_NOTE_READ_FULL, SettingAction::None));
  if (links) menuItems.push_back(SettingInfo::Action(StrId::STR_FOOTNOTES, SettingAction::None));
  if (siblings) {
    menuItems.push_back(SettingInfo::Action(StrId::STR_NOTE_PREVIOUS, SettingAction::None));
    menuItems.push_back(SettingInfo::Action(StrId::STR_NOTE_NEXT, SettingAction::None));
  }
  menuItems.push_back(SettingInfo::Action(StrId::STR_NOTE_RETURN_BOOK, SettingAction::None));
}

void FootnoteMenuActivity::onActionSelected(int index) {
  const auto id = menuItems[index].nameId;
  int action = RETURN_TO_BOOK;
  if (id == StrId::STR_CREATE_CLIPPING)
    action = HIGHLIGHT;
  else if (id == StrId::STR_LOOKUP)
    action = DICTIONARY;
  else if (id == StrId::STR_SELECT_DICTIONARY)
    action = SELECT_DICTIONARY;
  else if (id == StrId::STR_NOTE_READ_FULL)
    action = FULL_TEXT;
  else if (id == StrId::STR_FOOTNOTES)
    action = LINKS;
  else if (id == StrId::STR_NOTE_PREVIOUS)
    action = PREVIOUS_NOTE;
  else if (id == StrId::STR_NOTE_NEXT)
    action = NEXT_NOTE;
  MenuResult result;
  result.action = action;
  setResult(std::move(result));
  finish();
}

void FootnoteMenuActivity::render(RenderLock&&) {
  renderer.clearScreen();
  const Rect content = UITheme::getContentRect(renderer, true, true);
  renderer.drawCenteredText(UI_12_FONT_ID, content.y + 10, tr(STR_FOOTNOTES));
  drawMenuList(Rect{content.x, content.y + 50, content.width, content.height - 50});
  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_PREV), tr(STR_NEXT));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
  renderer.displayBuffer();
}

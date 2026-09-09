#include "ReadingProfilesActivity.h"

#include <GfxRenderer.h>
#include <I18n.h>

#include "components/UITheme.h"
#include "fontIds.h"

ReadingProfilesActivity::ReadingProfilesActivity(GfxRenderer& renderer, MappedInputManager& input)
    : MenuListActivity("ReadingProfiles", renderer, input) {
  // Four fixed choices; use the existing per-book override store rather than a second settings file.
  menuItems.reserve(4);
  menuItems.push_back(SettingInfo::Action(StrId::STR_PROFILE_NORMAL, SettingAction::None));
  menuItems.push_back(SettingInfo::Action(StrId::STR_PROFILE_LARGE, SettingAction::None));
  menuItems.push_back(SettingInfo::Action(StrId::STR_PROFILE_COMPACT, SettingAction::None));
  menuItems.push_back(SettingInfo::Action(StrId::STR_DEFAULT_VALUE, SettingAction::None));
}

void ReadingProfilesActivity::onActionSelected(int index) {
  MenuResult result;
  result.fontSizeOverride = index == 0   ? CrossPointSettings::MEDIUM
                            : index == 1 ? CrossPointSettings::EXTRA_LARGE
                            : index == 2 ? CrossPointSettings::SMALL
                                         : -1;
  result.lineHeightPercentOverride = index == 0 ? 100 : index == 1 ? 130 : index == 2 ? 90 : -1;
  setResult(std::move(result));
  finish();
}

std::string ReadingProfilesActivity::getItemValueString(int index) const {
  if (index == 0) return tr(STR_PROFILE_NORMAL_DETAIL);
  if (index == 1) return tr(STR_PROFILE_LARGE_DETAIL);
  if (index == 2) return tr(STR_PROFILE_COMPACT_DETAIL);
  return {};
}

void ReadingProfilesActivity::render(RenderLock&&) {
  renderer.clearScreen();
  const Rect content = UITheme::getContentRect(renderer, true, true);
  renderer.drawCenteredText(UI_12_FONT_ID, content.y + 10, tr(STR_READING_PROFILES));
  drawMenuList(Rect{content.x, content.y + 50, content.width, content.height - 50});
  const auto labels = mappedInput.mapLabels(tr(STR_BACK), tr(STR_SELECT), tr(STR_PREV), tr(STR_NEXT));
  GUI.drawButtonHints(renderer, labels.btn1, labels.btn2, labels.btn3, labels.btn4);
  renderer.displayBuffer();
}

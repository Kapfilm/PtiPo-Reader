#pragma once
#include "../MenuListActivity.h"

class ReadingProfilesActivity final : public MenuListActivity {
 public:
  ReadingProfilesActivity(GfxRenderer& renderer, MappedInputManager& input);
  void render(RenderLock&&) override;

 private:
  void onActionSelected(int index) override;
  std::string getItemValueString(int index) const override;
};

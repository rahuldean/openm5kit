#include "OpenM5Kit.h"

namespace openm5kit {

namespace {

const char* targetLabel(Target target) {
  switch (target) {
    case Target::M5Stick:
      return "M5Stick";
    case Target::CoreS3:
      return "CoreS3";
  }

  return "Unknown";
}

}  // namespace

App::App(const DeviceProfile& profile) : profile_(profile), lastDrawMs_(0) {}

void App::begin() {
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(profile_.rotation);
  M5.Display.setTextDatum(top_left);
  drawHomeScreen();
}

void App::update() {
  M5.update();

  const unsigned long now = millis();
  if (now - lastDrawMs_ >= 1000) {
    drawHomeScreen();
  }
}

void App::drawHomeScreen() {
  lastDrawMs_ = millis();

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(8, 8);
  M5.Display.print("OpenM5Kit");

  M5.Display.setTextSize(1);
  M5.Display.setCursor(8, 36);
  M5.Display.print("Target: ");
  M5.Display.print(profile_.name);

  M5.Display.setCursor(8, 52);
  M5.Display.print("Family: ");
  M5.Display.print(targetLabel(profile_.target));

  M5.Display.setCursor(8, 68);
  M5.Display.print("Uptime: ");
  M5.Display.print(lastDrawMs_ / 1000);
  M5.Display.print("s");

  M5.Display.setCursor(8, 84);
  M5.Display.print("Battery: ");
  M5.Display.print(M5.Power.getBatteryLevel());
  M5.Display.print("%");
}

DeviceProfile makeM5StickProfile() {
  return DeviceProfile{
      Target::M5Stick,
      "M5Stick",
      "stick",
      1,
  };
}

DeviceProfile makeCoreS3Profile() {
  return DeviceProfile{
      Target::CoreS3,
      "M5Stack CoreS3",
      "cores3",
      1,
  };
}

}  // namespace openm5kit

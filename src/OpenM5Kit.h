#pragma once

#include <M5Unified.h>

namespace openm5kit {

enum class Target {
  M5Stick,
  CoreS3,
};

struct DeviceProfile {
  Target target;
  const char* name;
  const char* shortName;
  int rotation;
};

class App {
 public:
  explicit App(const DeviceProfile& profile);

  void begin();
  void update();

 private:
  void drawHomeScreen();

  DeviceProfile profile_;
  unsigned long lastDrawMs_;
};

DeviceProfile makeM5StickProfile();
DeviceProfile makeCoreS3Profile();

}  // namespace openm5kit

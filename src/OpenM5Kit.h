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

struct DashboardConfig {
  bool enabled;
  const char* wifiSsid;
  const char* wifiPassword;
  const char* dashboardBaseUrl;
  const char* deviceId;
  const char* pairingCode;
  const char* firmwareVersion;
  unsigned long helloIntervalMs;
  unsigned long eventIntervalMs;
};

class App {
 public:
  explicit App(const DeviceProfile& profile);
  App(const DeviceProfile& profile, const DashboardConfig& dashboardConfig);

  void begin();
  void update();

 private:
  void beginNetwork();
  void drawHomeScreen();
  void sendHello();
  void sendTelemetry();
  bool postJson(const char* path, const String& payload);

  DeviceProfile profile_;
  DashboardConfig dashboardConfig_;
  unsigned long lastDrawMs_;
  unsigned long lastHelloMs_;
  unsigned long lastEventMs_;
  bool wifiConnected_;
  int lastPostStatus_;
};

DeviceProfile makeM5StickProfile();
DeviceProfile makeCoreS3Profile();

}  // namespace openm5kit

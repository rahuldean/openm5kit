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
  unsigned long messageIntervalMs;
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
  void requestRedraw();
  void sendHello();
  void sendTelemetry();
  void fetchMessage();
  bool postJson(const char* path, const String& payload);
  int getText(const char* path, String& responseBody);
  String dashboardUrl(const char* path) const;
  String messagePath() const;

  DeviceProfile profile_;
  DashboardConfig dashboardConfig_;
  unsigned long lastDrawMs_;
  unsigned long lastHelloMs_;
  unsigned long lastEventMs_;
  unsigned long lastMessageMs_;
  bool wifiConnected_;
  bool screenDirty_;
  int lastDisplayedBattery_;
  int lastDisplayedPostStatus_;
  int lastPostStatus_;
  int lastMessageStatus_;
  String lastPostPath_;
  String lastPostError_;
  String currentMessage_;
  String lastMessageError_;
};

DeviceProfile makeM5StickProfile();
DeviceProfile makeCoreS3Profile();

}  // namespace openm5kit

#include "../../src/OpenM5Kit.h"

#if __has_include("OpenM5KitConfig.h")
#include "OpenM5KitConfig.h"
#else
#include "OpenM5KitConfig.example.h"
#endif

openm5kit::DashboardConfig dashboardConfig{
    OPENM5KIT_DASHBOARD_ENABLED,
    OPENM5KIT_WIFI_SSID,
    OPENM5KIT_WIFI_PASSWORD,
    OPENM5KIT_DASHBOARD_BASE_URL,
    OPENM5KIT_DEVICE_ID,
    OPENM5KIT_PAIRING_CODE,
    OPENM5KIT_FIRMWARE_VERSION,
    OPENM5KIT_HELLO_INTERVAL_MS,
    OPENM5KIT_EVENT_INTERVAL_MS,
    OPENM5KIT_MESSAGE_INTERVAL_MS,
};

openm5kit::App app(openm5kit::makeCoreS3Profile(), dashboardConfig);

void setup() {
  app.begin();
}

void loop() {
  app.update();
}

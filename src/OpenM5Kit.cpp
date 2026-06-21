#include "OpenM5Kit.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>

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

bool hasValue(const char* value) {
  return value != nullptr && value[0] != '\0';
}

bool isDashboardConfigured(const DashboardConfig& config) {
  return config.enabled && hasValue(config.wifiSsid) &&
         hasValue(config.dashboardBaseUrl) && hasValue(config.deviceId) &&
         hasValue(config.pairingCode);
}

String baseDeviceJson(const DeviceProfile& profile,
                      const DashboardConfig& config) {
  String payload = "{";
  payload += "\"deviceId\":\"";
  payload += config.deviceId;
  payload += "\",\"deviceType\":\"";
  payload += profile.shortName;
  payload += "\",\"deviceName\":\"";
  payload += profile.name;
  payload += "\",\"firmwareVersion\":\"";
  payload += config.firmwareVersion;
  payload += "\",\"pairingCode\":\"";
  payload += config.pairingCode;
  payload += "\"";
  return payload;
}

}  // namespace

App::App(const DeviceProfile& profile)
    : App(profile,
          DashboardConfig{
              false,
              "",
              "",
              "",
              "",
              "",
              "0.1.0",
              30000,
              5000,
          }) {}

App::App(const DeviceProfile& profile, const DashboardConfig& dashboardConfig)
    : profile_(profile),
      dashboardConfig_(dashboardConfig),
      lastDrawMs_(0),
      lastHelloMs_(0),
      lastEventMs_(0),
      wifiConnected_(false),
      lastPostStatus_(0),
      lastPostPath_("-"),
      lastPostError_("-") {}

void App::begin() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("OpenM5Kit boot");

  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(profile_.rotation);
  M5.Display.setTextDatum(top_left);
  beginNetwork();
  drawHomeScreen();
}

void App::update() {
  M5.update();

  if (isDashboardConfigured(dashboardConfig_)) {
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected_ = false;
      WiFi.reconnect();
    } else {
      wifiConnected_ = true;
    }

    const unsigned long now = millis();
    if (wifiConnected_ && now - lastHelloMs_ >= dashboardConfig_.helloIntervalMs) {
      sendHello();
    }
    if (wifiConnected_ && now - lastEventMs_ >= dashboardConfig_.eventIntervalMs) {
      sendTelemetry();
    }
  }

  const unsigned long now = millis();
  if (now - lastDrawMs_ >= 1000) {
    drawHomeScreen();
  }
}

void App::beginNetwork() {
  if (!isDashboardConfigured(dashboardConfig_)) {
    return;
  }

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(dashboardConfig_.wifiSsid);
  WiFi.begin(dashboardConfig_.wifiSsid, dashboardConfig_.wifiPassword);

  const unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 15000) {
    delay(250);
    M5.update();
  }

  wifiConnected_ = WiFi.status() == WL_CONNECTED;
  if (wifiConnected_) {
    Serial.print("WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Dashboard: ");
    Serial.println(dashboardConfig_.dashboardBaseUrl);
    sendHello();
    sendTelemetry();
  } else {
    Serial.println("WiFi connection timed out.");
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

  M5.Display.setCursor(8, 100);
  M5.Display.print("WiFi: ");
  if (!isDashboardConfigured(dashboardConfig_)) {
    M5.Display.print("not configured");
  } else if (wifiConnected_) {
    M5.Display.print(WiFi.localIP());
  } else {
    M5.Display.print("disconnected");
  }

  M5.Display.setCursor(8, 116);
  M5.Display.print("Pair: ");
  if (hasValue(dashboardConfig_.pairingCode)) {
    M5.Display.print(dashboardConfig_.pairingCode);
  } else {
    M5.Display.print("-");
  }

  M5.Display.setCursor(8, 132);
  M5.Display.print("API: ");
  M5.Display.print(lastPostStatus_);

  if (lastPostStatus_ < 0) {
    M5.Display.setCursor(8, 148);
    M5.Display.print("Err: ");
    M5.Display.print(lastPostError_);
  }
}

void App::sendHello() {
  lastHelloMs_ = millis();

  String payload = baseDeviceJson(profile_, dashboardConfig_);
  payload += ",\"ipAddress\":\"";
  payload += WiFi.localIP().toString();
  payload += "\",\"rssi\":";
  payload += WiFi.RSSI();
  payload += ",\"battery\":";
  payload += M5.Power.getBatteryLevel();
  payload += ",\"uptimeMs\":";
  payload += millis();
  payload += "}";

  postJson("/api/devices/hello", payload);
}

void App::sendTelemetry() {
  lastEventMs_ = millis();

  String payload = baseDeviceJson(profile_, dashboardConfig_);
  payload += ",\"eventType\":\"telemetry\",\"ipAddress\":\"";
  payload += WiFi.localIP().toString();
  payload += "\",\"rssi\":";
  payload += WiFi.RSSI();
  payload += ",\"battery\":";
  payload += M5.Power.getBatteryLevel();
  payload += ",\"uptimeMs\":";
  payload += millis();
  payload += ",\"freeHeap\":";
  payload += ESP.getFreeHeap();
  payload += ",\"buttonA\":";
  payload += (M5.BtnA.isPressed() ? "true" : "false");
  payload += ",\"buttonB\":";
  payload += (M5.BtnB.isPressed() ? "true" : "false");
  payload += "}";

  postJson("/api/devices/events", payload);
}

bool App::postJson(const char* path, const String& payload) {
  if (!wifiConnected_) {
    return false;
  }

  WiFiClient client;
  HTTPClient http;
  String url = dashboardUrl(path);
  lastPostPath_ = path;

  http.setTimeout(5000);
  http.useHTTP10(true);
  if (!http.begin(client, url)) {
    lastPostStatus_ = -1;
    lastPostError_ = "http.begin failed";
    Serial.print("Dashboard request failed: ");
    Serial.println(lastPostError_);
    return false;
  }
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "close");
  lastPostStatus_ = http.POST(payload);
  if (lastPostStatus_ < 0) {
    lastPostError_ = http.errorToString(lastPostStatus_);
  } else {
    lastPostError_ = "OK";
  }

  if (lastPostStatus_ < 200 || lastPostStatus_ >= 300) {
    Serial.print("Dashboard request failed: ");
    Serial.print(lastPostPath_);
    Serial.print(" status=");
    Serial.print(lastPostStatus_);
    Serial.print(" error=");
    Serial.println(lastPostError_);
  }
  http.end();

  return lastPostStatus_ >= 200 && lastPostStatus_ < 300;
}

String App::dashboardUrl(const char* path) const {
  String baseUrl = dashboardConfig_.dashboardBaseUrl;
  while (baseUrl.endsWith("/")) {
    baseUrl.remove(baseUrl.length() - 1);
  }

  String normalizedPath = path;
  if (!normalizedPath.startsWith("/")) {
    normalizedPath = "/" + normalizedPath;
  }

  return baseUrl + normalizedPath;
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

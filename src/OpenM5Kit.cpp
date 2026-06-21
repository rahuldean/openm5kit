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
              3000,
              60000,
          }) {}

App::App(const DeviceProfile& profile, const DashboardConfig& dashboardConfig)
    : profile_(profile),
      dashboardConfig_(dashboardConfig),
      lastDrawMs_(0),
      lastHelloMs_(0),
      lastEventMs_(0),
      lastMessageMs_(0),
      lastInteractionMs_(0),
      wifiConnected_(false),
      screenAwake_(true),
      screenDirty_(true),
      lastDisplayedBattery_(-1),
      lastDisplayedPostStatus_(0),
      lastPostStatus_(0),
      lastMessageStatus_(0),
      lastPostPath_("-"),
      lastPostError_("-"),
      currentMessage_("Message Board Ready"),
      lastMessageError_("-") {}

void App::begin() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("OpenM5Kit boot");

  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(profile_.rotation);
  M5.Display.setTextDatum(top_left);
  recordInteraction();
  beginNetwork();
  drawHomeScreen();
}

void App::update() {
  M5.update();
  const unsigned long now = millis();

  if (M5.Touch.getCount() > 0) {
    if (!screenAwake_) {
      wakeScreen();
    }
    recordInteraction();
  }

  if (M5.BtnPWR.wasClicked() && screenAwake_) {
    sleepScreen();
  }

  if (isDashboardConfigured(dashboardConfig_)) {
    const bool wasWifiConnected = wifiConnected_;
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected_ = false;
      WiFi.reconnect();
    } else {
      wifiConnected_ = true;
    }
    if (wifiConnected_ != wasWifiConnected) {
      requestRedraw();
    }

    if (wifiConnected_ && now - lastHelloMs_ >= dashboardConfig_.helloIntervalMs) {
      sendHello();
    }
    if (wifiConnected_ && now - lastEventMs_ >= dashboardConfig_.eventIntervalMs) {
      sendTelemetry();
    }
    if (wifiConnected_ && now - lastMessageMs_ >= dashboardConfig_.messageIntervalMs) {
      fetchMessage();
    }
  }

  const unsigned long sleepCheckMs = millis();
  if (screenAwake_ && dashboardConfig_.screenTimeoutMs > 0 &&
      sleepCheckMs - lastInteractionMs_ >= dashboardConfig_.screenTimeoutMs) {
    sleepScreen();
  }

  const unsigned long redrawCheckMs = millis();
  const bool needsSlowStatusRefresh = redrawCheckMs - lastDrawMs_ >= 30000;
  const bool batteryChanged =
      abs(M5.Power.getBatteryLevel() - lastDisplayedBattery_) >= 2;
  if (screenAwake_ && (screenDirty_ || needsSlowStatusRefresh || batteryChanged)) {
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
    fetchMessage();
  } else {
    Serial.println("WiFi connection timed out.");
  }
}

void App::drawHomeScreen() {
  if (!screenAwake_) {
    return;
  }

  lastDrawMs_ = millis();
  screenDirty_ = false;
  lastDisplayedBattery_ = M5.Power.getBatteryLevel();
  lastDisplayedPostStatus_ = lastPostStatus_;

  const int width = M5.Display.width();
  const int height = M5.Display.height();

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.fillRoundRect(8, 8, width - 16, 34, 8, TFT_DARKGREY);
  M5.Display.setTextColor(TFT_WHITE, TFT_DARKGREY);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(18, 20);
  M5.Display.print("OpenM5Kit");

  M5.Display.setCursor(width - 96, 20);
  M5.Display.print(wifiConnected_ ? "Online" : "Offline");

  M5.Display.fillRoundRect(8, 52, width - 16, height - 92, 10, TFT_WHITE);
  M5.Display.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(22, 70);
  M5.Display.print("Message");

  M5.Display.setTextSize(2);
  M5.Display.setCursor(22, 108);
  String message = currentMessage_;
  const int maxCharsPerLine = max(8, (width - 44) / 12);
  int line = 0;
  while (message.length() > 0 && line < 5) {
    int take = min(maxCharsPerLine, static_cast<int>(message.length()));
    int breakIndex = message.lastIndexOf(' ', take);
    if (breakIndex <= 0 || take == static_cast<int>(message.length())) {
      breakIndex = take;
    }

    String chunk = message.substring(0, breakIndex);
    chunk.trim();
    M5.Display.setCursor(22, 108 + line * 26);
    M5.Display.print(chunk);
    message = message.substring(breakIndex);
    message.trim();
    line++;
  }

  M5.Display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.setCursor(12, height - 28);
  M5.Display.print("Battery ");
  M5.Display.print(lastDisplayedBattery_);
  M5.Display.print("%");
  M5.Display.setCursor(width - 96, height - 28);
  M5.Display.print("API ");
  M5.Display.print(lastDisplayedPostStatus_);
}

void App::sleepScreen() {
  if (!screenAwake_) {
    return;
  }

  screenAwake_ = false;
  screenDirty_ = true;
  M5.Display.sleep();
  Serial.println("Display sleeping");
}

void App::wakeScreen() {
  if (screenAwake_) {
    return;
  }

  M5.Display.wakeup();
  screenAwake_ = true;
  recordInteraction();
  requestRedraw();
  Serial.println("Display awake");
}

void App::recordInteraction() {
  lastInteractionMs_ = millis();
}

void App::requestRedraw() {
  screenDirty_ = true;
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

void App::fetchMessage() {
  lastMessageMs_ = millis();

  String responseBody;
  const int status = getText(messagePath().c_str(), responseBody);
  lastMessageStatus_ = status;

  if (status == 200) {
    responseBody.trim();
    if (responseBody.length() > 0) {
      currentMessage_ = responseBody.substring(0, 240);
      Serial.print("Received message: ");
      Serial.println(currentMessage_);
      wakeScreen();
      recordInteraction();
      requestRedraw();
    }
  }
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
  if (lastPostStatus_ != lastDisplayedPostStatus_) {
    requestRedraw();
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

int App::getText(const char* path, String& responseBody) {
  if (!wifiConnected_) {
    return -1;
  }

  WiFiClient client;
  HTTPClient http;
  String url = dashboardUrl(path);

  http.setTimeout(5000);
  http.useHTTP10(true);
  if (!http.begin(client, url)) {
    lastMessageError_ = "http.begin failed";
    Serial.print("Message request failed: ");
    Serial.println(lastMessageError_);
    return -1;
  }

  http.addHeader("Connection", "close");
  const int status = http.GET();
  if (status == 200) {
    responseBody = http.getString();
  } else if (status < 0) {
    lastMessageError_ = http.errorToString(status);
    Serial.print("Message request failed: status=");
    Serial.print(status);
    Serial.print(" error=");
    Serial.println(lastMessageError_);
  }

  http.end();
  return status;
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

String App::messagePath() const {
  String path = "/api/devices/";
  path += dashboardConfig_.deviceId;
  path += "/message";
  return path;
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

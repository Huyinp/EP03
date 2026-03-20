/*
  心灵鸡汤 - ESP32 墨水屏显示
  2.7寸三色屏 (176x264)
  通过 WiFi 在线获取励志语录，定时自动刷新

  依赖库：
  - GxEPD2
  - Adafruit_GFX_Library
  - U8g2_for_Adafruit_GFX
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "bitmaps/Bitmaps3c176x264.h"

const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET = 8 * 3600;
const int DAYLIGHT_OFFSET = 0;

const char* WIFI_SSID = "Guest";
const char* WIFI_PASSWORD = "shunwang1122";

#define REFRESH_INTERVAL 60000

#define EPD2_COLOR GxEPD2_3C
#define EPD2_MODE GxEPD2_270c

#if defined(ESP32) && defined(CONFIG_IDF_TARGET_ESP32)
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(5, 17, 16, 19));
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(7, 3, 2, 10));
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(10, 8, 7, 9));
#else
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(15, 4, 2, 5));
#endif

U8G2_FOR_ADAFRUIT_GFX u8g2fonts;

char soupText[512] = "正在获取语录...";
unsigned long lastRefreshTime = 0;
bool hasUpdate = false;

char yearMonthStr[12] = "";
int currentDay = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== 心灵鸡汤 ESP32 墨水屏 ===");

  setupWiFi();
  initDisplay();
  fetchSoupText();

  lastRefreshTime = millis();
  hasUpdate = true;
  Serial.println("初始化完成");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastRefreshTime >= REFRESH_INTERVAL) {
    Serial.println("定时刷新...");
    fetchSoupText();
    updateDateTime();
    hasUpdate = true;
    lastRefreshTime = currentTime;
  }

  if (hasUpdate) {
    drawScreen();
    hasUpdate = false;
  }

  delay(100);
}

void setupWiFi() {
  Serial.print("连接 WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi 连接成功!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
    delay(1000);
    updateDateTime();
  } else {
    Serial.println();
    Serial.println("WiFi 连接失败!");
    strcpy(soupText, "WiFi 连接失败，请检查配置");
  }
}

void initDisplay() {
  display.init();
  display.setRotation(2);
  display.setFullWindow();
  u8g2fonts.begin(display);
}

void updateDateTime() {
  struct tm timeinfo;
  for (int i = 0; i < 5; i++) {
    if (getLocalTime(&timeinfo)) {
      int year2 = (timeinfo.tm_year + 1900) % 100;
      sprintf(yearMonthStr, "%02d/%02d", year2, timeinfo.tm_mon + 1);
      currentDay = timeinfo.tm_mday;
      Serial.println("时间: " + String(yearMonthStr) + " " + String(currentDay));
      return;
    }
    delay(500);
  }
  Serial.println("时间同步失败");
}

void fetchSoupText() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 未连接");
    return;
  }

  HTTPClient http;
  const char* urls[] = {
    "https://v1.jinrishici.com/rensheng.txt",
    "https://zj.v.api.aa1.cn/api/wenan-zl/?type=json",
    "https://api.ooopn.com/ciba/index.php?type=text",
    "https://api.xygeng.cn/Gr saying",
    "https://international.v1.hssk.cn/random",
    "https://api.suxun.io/api/rensheng",
    "https://api.77tianapi.com/renshengtxt/index.php"
  };

  int urlCount = sizeof(urls) / sizeof(urls[0]);
  int startIdx = random(0, urlCount);

  int httpCode = 0;
  String payload = "";

  for (int i = 0; i < urlCount; i++) {
    int idx = (startIdx + i) % urlCount;
    http.begin(urls[idx]);
    httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK || httpCode == 200) {
      payload = http.getString();
      break;
    }
    http.end();
  }

  if (httpCode == HTTP_CODE_OK || httpCode == 200) {
    Serial.println("API: " + payload);
    payload.toCharArray(soupText, sizeof(soupText));
    Serial.println("语录: " + String(soupText));
  } else {
    Serial.print("HTTP 错误: ");
    Serial.println(httpCode);
    strcpy(soupText, "网络请求失败");
  }

  http.end();
}

void drawScreen() {
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.drawRect(5, 5, 166, 254, GxEPD_BLACK);

    u8g2fonts.setFont(u8g2_font_helvR10_tn);
    u8g2fonts.setForegroundColor(GxEPD_BLACK);
    u8g2fonts.setBackgroundColor(GxEPD_WHITE);
    u8g2fonts.setCursor(18, 18);
    u8g2fonts.print(yearMonthStr);

    u8g2fonts.setFont(u8g2_font_logisoso58_tn);
    u8g2fonts.setForegroundColor(GxEPD_RED);
    u8g2fonts.setBackgroundColor(GxEPD_WHITE);
    char dayStr[4];
    sprintf(dayStr, "%d", currentDay);
    int16_t dayWidth = u8g2fonts.getUTF8Width(dayStr);
    int16_t dayX = (176 - dayWidth) / 2;
    u8g2fonts.setCursor(dayX, 115);
    u8g2fonts.print(dayStr);

    display.drawLine(30, 150, 146, 150, GxEPD_BLACK);

    drawWrappedText(soupText, 175, 155, GxEPD_BLACK);

  } while (display.nextPage());
}

void drawWrappedText(const char* text, int startY, int maxWidth, uint16_t color) {
  u8g2fonts.setFont(u8g2_font_wqy12_t_gb2312b);
  u8g2fonts.setForegroundColor(color);
  u8g2fonts.setBackgroundColor(GxEPD_WHITE);

  int x = 18;
  int y = startY;
  int lineHeight = 17;
  int chineseWidth = 14;
  int asciiWidth = 7;

  const char* ptr = text;
  int currentX = x;

  while (*ptr) {
    int charLen = 1;
    int charW = asciiWidth;

    if ((*ptr & 0x80) == 0) {
      charW = asciiWidth;
    } else if ((*ptr & 0xE0) == 0xC0) {
      charLen = 2;
      charW = chineseWidth;
    } else if ((*ptr & 0xF0) == 0xE0) {
      charLen = 3;
      charW = chineseWidth;
    } else if ((*ptr & 0xF8) == 0xF0) {
      charLen = 4;
      charW = chineseWidth * 1.3;
    } else {
      ptr++;
      continue;
    }

    char buf[5] = {0};
    for (int i = 0; i < charLen && ptr[i]; i++) {
      buf[i] = ptr[i];
    }

    int glyphW = u8g2fonts.getUTF8Width(buf);

    if (glyphW == 0 || glyphW >= chineseWidth * 1.5) {
      ptr += charLen;
      continue;
    }

    if (currentX + glyphW * 1.2 > maxWidth) {
      y += lineHeight;
      currentX = x;
      if (y > 250) break;
    }

    u8g2fonts.setCursor(currentX, y);
    u8g2fonts.print(buf);

    currentX += glyphW * 1.2;
    ptr += charLen;
  }
}

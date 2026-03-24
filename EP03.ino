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

const char* NTP_SERVER = "ntp5.aliyun.com";
const long GMT_OFFSET = 8 * 3600;
const int DAYLIGHT_OFFSET = 0;

const char* WIFI_SSID = "Guest";
const char* WIFI_PASSWORD = "shunwang1122";

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

// 时间相关
char headerStr[32] = "Jan 2025";      // 顶部年月
char dayStr[8] = "11";                // 日期数字
char weekdayStr[16] = "Monday";       // 星期几
char progressStr[32] = "";            // 年度进度

// 语录相关
char quoteText[256] = "生活不止眼前的苟且，还有诗和远方。";
int currentDay = 0;

// 上证指数相关
char indexStr[32] = "SH:----";
char cybStr[32] = "CY:----";  // 创业板指数

// 星期名称
const char* WEEKDAYS[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void setup() {
  Serial.begin(115200);
  delay(1000);  // 等待串口稳定
  Serial.println();
  Serial.println("=== 心灵鸡汤 ESP32 墨水屏 ===");

  initDisplay();
  yield();  // 喂狗
  
  setupWiFi();
  yield();
  
  updateDateTime();
  yield();
  
  fetchQuote();
  yield();
  
  fetchIndex();
  yield();
  
  drawScreen();
  yield();

  Serial.println("显示完成");
}

void loop() {
  delay(60000);  // 60秒后重新获取
  yield();
  updateDateTime();
  yield();
  fetchQuote();
  yield();
  fetchIndex();
  yield();
  drawScreen();
  yield();
}

void initDisplay() {
  display.init(0, false, 2, true);  // 使用更稳定的初始化参数
  display.setRotation(2);
  display.setFullWindow();
  u8g2fonts.begin(display);
  delay(100);
  yield();
}

void setupWiFi() {
  Serial.print("连接 WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    yield();  // 喂狗
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi 连接成功!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
    delay(500);
  } else {
    Serial.println();
    Serial.println("WiFi 连接失败!");
    strcpy(quoteText, "WiFi 连接失败");
  }
  yield();
}

void updateDateTime() {
  struct tm timeinfo;
  for (int i = 0; i < 5; i++) {
    if (getLocalTime(&timeinfo)) {
      // 格式化年月 "Mar 2026"
      sprintf(headerStr, "%s %d", MONTHS[timeinfo.tm_mon], timeinfo.tm_year + 1900);
      
      // 日期数字
      sprintf(dayStr, "%d", timeinfo.tm_mday);
      currentDay = timeinfo.tm_mday;
      
      // 星期几
      strcpy(weekdayStr, WEEKDAYS[timeinfo.tm_wday]);
      
      // 年度进度
      int dayOfYear = timeinfo.tm_yday;
      float progress = (float)dayOfYear / 365.0 * 100.0;
      sprintf(progressStr, "%d is %.1f%% complete", timeinfo.tm_year + 1900, progress);
      
      Serial.println("时间: " + String(headerStr) + " " + String(dayStr) + " " + String(weekdayStr));
      return;
    }
    delay(500);
  }
  Serial.println("时间同步失败");
}

void fetchQuote() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 未连接");
    return;
  }

  HTTPClient http;
  http.setTimeout(5000);  // 5秒超时
  
  // 只使用一个可靠的API，减少内存占用
  const char* url = "https://v1.jinrishici.com/rensheng.txt";
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("API: " + payload);
    
    // 直接使用返回的文本
    if (payload.length() > 0) {
      // 限制长度
      if (payload.length() > 50) {
        payload = payload.substring(0, 50);
        int lastPeriod = payload.lastIndexOf('。');
        if (lastPeriod > 20) {
          payload = payload.substring(0, lastPeriod + 1);
        }
      }
      payload.toCharArray(quoteText, sizeof(quoteText));
      Serial.println("语录: " + String(quoteText));
    }
  } else {
    Serial.print("HTTP 错误: ");
    Serial.println(httpCode);
    strcpy(quoteText, "今天也要开心鸭！");
  }
  
  http.end();
  yield();
}

void fetchIndex() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi 未连接，跳过指数获取");
    return;
  }

  HTTPClient http;
  http.setTimeout(5000);
  
  // 获取上证指数
  http.begin("https://qt.gtimg.cn/q=sh000001");
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("SH原始:" + payload);
    
    // 解析格式: v_sh000001="1~上证指数~000001~3862.62~..."
    int t1 = payload.indexOf("~");
    int t2 = payload.indexOf("~", t1 + 1);
    int t3 = payload.indexOf("~", t2 + 1);
    int t4 = payload.indexOf("~", t3 + 1);
    
    if (t3 != -1 && t4 != -1) {
      String price = payload.substring(t3 + 1, t4);
      sprintf(indexStr, "SH:%s", price.c_str());
      Serial.println("上证:" + String(indexStr));
    }
  } else {
    Serial.print("上证HTTP错误: ");
    Serial.println(httpCode);
    strcpy(indexStr, "SH:----");
  }
  http.end();
  yield();
  
  // 获取创业板指数
  http.begin("https://qt.gtimg.cn/q=sz399006");
  httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("CY原始:" + payload);
    
    // 解析格式: v_sz399006="1~创业板指~399006~2156.32~..."
    int t1 = payload.indexOf("~");
    int t2 = payload.indexOf("~", t1 + 1);
    int t3 = payload.indexOf("~", t2 + 1);
    int t4 = payload.indexOf("~", t3 + 1);
    
    if (t3 != -1 && t4 != -1) {
      String price = payload.substring(t3 + 1, t4);
      sprintf(cybStr, "CY:%s", price.c_str());
      Serial.println("创业板:" + String(cybStr));
    }
  } else {
    Serial.print("创业板HTTP错误: ");
    Serial.println(httpCode);
    strcpy(cybStr, "CY:----");
  }
  
  http.end();
  yield();
}

void drawScreen() {
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);

    // 1. 顶部年月（居中显示）
    u8g2fonts.setFont(u8g2_font_helvR10_tr);
    u8g2fonts.setForegroundColor(GxEPD_BLACK);
    u8g2fonts.setBackgroundColor(GxEPD_WHITE);
    int16_t headerWidth = u8g2fonts.getUTF8Width(headerStr);
    int16_t headerX = (176 - headerWidth) / 2;
    u8g2fonts.setCursor(headerX, 25);
    u8g2fonts.print(headerStr);

    // 2. 日期数字（红色大字，居中显示）
    u8g2fonts.setFont(u8g2_font_logisoso58_tn);
    u8g2fonts.setForegroundColor(GxEPD_RED);
    int16_t dayWidth = u8g2fonts.getUTF8Width(dayStr);
    int16_t dayX = (176 - dayWidth) / 2;
    u8g2fonts.setCursor(dayX, 95);
    u8g2fonts.print(dayStr);

    // 3. 星期几（居中显示）
    u8g2fonts.setFont(u8g2_font_helvR10_tr);
    u8g2fonts.setForegroundColor(GxEPD_BLACK);
    int16_t weekdayWidth = u8g2fonts.getUTF8Width(weekdayStr);
    int16_t weekdayX = (176 - weekdayWidth) / 2;
    u8g2fonts.setCursor(weekdayX, 130);
    u8g2fonts.print(weekdayStr);

    // 4. 年度进度（居中显示，小字体）
    u8g2fonts.setFont(u8g2_font_helvR08_tr);
    int16_t progressWidth = u8g2fonts.getUTF8Width(progressStr);
    int16_t progressX = (176 - progressWidth) / 2;
    u8g2fonts.setCursor(progressX, 150);
    u8g2fonts.print(progressStr);

    // 5. 分隔线
    display.drawLine(30, 165, 146, 165, GxEPD_BLACK);

    // 6. 语录区域（左对齐，最多3行）
    u8g2fonts.setFont(u8g2_font_wqy12_t_gb2312b);
    u8g2fonts.setForegroundColor(GxEPD_BLACK);
    drawQuoteText(quoteText, 190, 20);

    // 7. 上证指数和创业板指数（底部显示）
    u8g2fonts.setFont(u8g2_font_helvR08_tr);
    u8g2fonts.setForegroundColor(GxEPD_BLACK);
    u8g2fonts.setCursor(10, 255);
    u8g2fonts.print(indexStr);
    u8g2fonts.setCursor(90, 255);
    u8g2fonts.print(cybStr);

  } while (display.nextPage());
}

void drawQuoteText(const char* text, int startY, int lineHeight) {
  u8g2fonts.setFont(u8g2_font_wqy12_t_gb2312b);
  u8g2fonts.setForegroundColor(GxEPD_BLACK);
  u8g2fonts.setBackgroundColor(GxEPD_WHITE);

  int maxWidth = 130;  // 最大宽度（增加边距）
  int x = 23;          // 起始X坐标（增加边距）
  int y = startY;
  int lineCount = 0;
  int maxLines = 3;    // 最多3行

  const char* ptr = text;
  int currentX = x;
  char lineBuf[64] = {0};
  int lineIdx = 0;

  while (*ptr && lineCount < maxLines) {
    int charLen = 1;
    int charW = 7;

    // 判断UTF-8字符长度
    if ((*ptr & 0x80) == 0) {
      charLen = 1;
      charW = 7;
    } else if ((*ptr & 0xE0) == 0xC0) {
      charLen = 2;
      charW = 14;
    } else if ((*ptr & 0xF0) == 0xE0) {
      charLen = 3;
      charW = 14;
    } else if ((*ptr & 0xF8) == 0xF0) {
      charLen = 4;
      charW = 14;
    } else {
      ptr++;
      continue;
    }

    // 复制字符到缓冲区
    char buf[5] = {0};
    for (int i = 0; i < charLen && ptr[i]; i++) {
      buf[i] = ptr[i];
    }

    int glyphW = u8g2fonts.getUTF8Width(buf);

    // 换行检查
    if (currentX + glyphW > x + maxWidth) {
      // 绘制当前行（左对齐）
      lineBuf[lineIdx] = 0;
      u8g2fonts.setCursor(x, y);
      u8g2fonts.print(lineBuf);
      
      // 新行
      lineCount++;
      y += lineHeight;
      currentX = x;
      lineIdx = 0;
      lineBuf[0] = 0;
      
      if (lineCount >= maxLines) break;
    }

    // 添加字符到当前行
    for (int i = 0; i < charLen; i++) {
      lineBuf[lineIdx++] = ptr[i];
    }
    currentX += glyphW;
    ptr += charLen;
  }

  // 绘制最后一行（左对齐）
  if (lineIdx > 0 && lineCount < maxLines) {
    lineBuf[lineIdx] = 0;
    u8g2fonts.setCursor(x, y);
    u8g2fonts.print(lineBuf);
  }
}

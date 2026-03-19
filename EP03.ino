/*
  WFT0190CZ22 2.7寸屏三色驱动类型 示例程序
  分辨率 176*264
  屏资料 微雪 2.7B V1 ： https://www.waveshare.net/wiki/2.7inch_e-Paper_HAT_(B)_Manual

*/

#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "bitmaps/Bitmaps3c176x264.h"
#include "imgdata.h"

// 屏配置
// 颜色选择 双色 GxEPD2_3C  三色 GxEPD2_3C
#define EPD2_COLOR GxEPD2_3C
// 屏型号配置
#define EPD2_MODE GxEPD2_270c

#ifndef EPD2_COLOR
#define EPD2_COLOR GxEPD2_3C
#endif

#ifndef EPD2_MODE
#define EPD2_MODE GxEPD2_270c
#endif

// 墨水屏类型定义
#if defined(ESP32)
#if defined(CONFIG_IDF_TARGET_ESP32)
// ESP32主控接法：SS(CS)=5 MOSI(SDA)=23  MISO(不接)=19 SCK=18 DC=17 RST=16 BUSY=19
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(/*CS=5*/ 5, /*DC=*/17, /*RST=*/16, /*BUSY=*/19));
#endif

#if defined(CONFIG_IDF_TARGET_ESP32C3)
// ESP32C3主控接法：SS(CS)=7 MOSI(SDA)=6  MISO(不接)=5 SCK=4 DC=3 RST=2 BUSY=10
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(/*CS=*/7, /*DC=*/3, /*RST=*/2, /*BUSY=*/10));
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
// ESP32S3主控接法：SS(CS)=10 MOSI(SDA)=11  MISO(不接)=13 SCK=12 DC=8 RST=7 BUSY=9
EPD2_COLOR<EPD2_MODE, EPD2_MODE::HEIGHT> display(EPD2_MODE(/*CS=*/10, /*DC=*/8, /*RST=*/7, /*BUSY=*/9));
#endif

#else

// ESP8266 模块接法：SS(CS)=15 MOSI(SDA)=13  MISO=12 SCK(SCLK)=14 DC=4 RST=2 BUSY=5
// ESP8266开发板对应引脚: CS=D8 SDA=D7 SCK=D5 DC=D2 RST=D4 BUSY=D1
// YMS152152 1.54寸黑白红三色
EPD2_COLOR<EPD2_MODE, EPD2_MODE ::HEIGHT> display(EPD2_MODE(/*CS=5*/ 15, /*DC=*/4, /*RST=*/2, /*BUSY=*/5));

#endif


char HelloWorld[] = "Hello World!";

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.println("Ink Test Start ...\n");
  Serial.println();

  // 墨水屏初始化
  display.init();  // default 20ms reset pulse
  // 设置旋转方向
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  // 设置文字颜色
  display.setTextColor(GxEPD_BLACK);
  // 设置全刷
  display.setFullWindow();
  display.firstPage();
  
  do {
    // 清屏，以白色填充
    display.fillScreen(GxEPD_WHITE);
    // // 设置开始显示的位置
    // display.setCursor(10, 10);
    // // 显示文字内容
    // display.print(HelloWorld);

    // // 如果是三色屏，设置为红色字体，否则以黑色显示
    // display.setTextColor(display.epd2.hasColor ? GxEPD_YELLOW : GxEPD_BLACK);
    // // 设置开始显示的位置
    // display.setCursor(10, 10 + 30);
    // // 显示文字内容
    // display.print(HelloWorld);

    display.drawInvertedBitmap(0, 0, gImage_BW1, 176, 264, GxEPD_BLACK);
    display.drawInvertedBitmap(0, 0, gImage_RW1, 176, 264, GxEPD_RED);

  } while (display.nextPage());
}

void loop() {
  // put your main code here, to run repeatedly:
}

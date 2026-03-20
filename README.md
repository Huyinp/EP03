# 心灵鸡汤 - ESP32 墨水屏显示

2.7寸三色墨水屏显示励志语录，通过 WiFi 在线获取，每日自动刷新。

## 功能特性

- 在线获取励志语录/诗词
- NTP 自动同步日期时间
- 支持三色显示（黑、红、白）
- 自动跳过字库不支持的字符
- 多 API 随机切换

## 硬件要求

- ESP32 开发板
- 2.7寸三色墨水屏（176x264）
- 推荐 4MB Flash 以上

## 引脚连接

| 墨水屏 | ESP32 |
|--------|-------|
| BUSY   | GPIO5 |
| RST    | GPIO17|
| DC     | GPIO16|
| CS     | GPIO19|
| CLK    | GPIO18|
| DIN    | GPIO23|
| GND    | GND   |
| VCC    | 3.3V  |

## 配置

编辑 `EP03.ino` 修改 WiFi 信息：

```cpp
const char* WIFI_SSID = "你的WiFi名称";
const char* WIFI_PASSWORD = "你的WiFi密码";
```

## 编译上传

1. 安装依赖库：
   - GxEPD2
   - Adafruit_GFX_Library
   - U8g2_for_Adafruit_GFX

2. 使用 Arduino IDE 或 arduino-cli 编译上传

3. 打开串口监视器（115200 baud）查看运行状态

## 刷新间隔

默认 60 秒刷新一次，修改 `REFRESH_INTERVAL` 可调整：

```cpp
#define REFRESH_INTERVAL 60000  // 毫秒
```

## 显示布局

```
┌─────────────────────┐
│ 26/03               │  ← 年月（居左）
│                     │
│         20          │  ← 日期（红色大字）
│─────────────────────│
│                     │
│   语录内容显示      │  ← 语录区域
│   自动换行          │
│                     │
└─────────────────────┘
```

## API 来源

- 今日诗词 (v1.jinrishici.com)
- 励志语录 API
- 其他备用接口

## 字体说明

使用 WQY 中文字库，支持大部分常用汉字。部分生僻字会自动跳过。

# 项目规则

## 技术栈
- **平台**: Arduino (ESP32)
- **开发板**: ESP32 (ESP32_DEV)
- **核心库**:
  - GxEPD2 - 电子墨水屏驱动
  - Adafruit_GFX_Library - 图形库
  - Adafruit_BusIO - 总线通信库
- **编译器**: xtensa-esp32-elf-g++ (ESP32 工具链)
- **C++标准**: C++17
- **Arduino API版本**: 1.06.07

---

## 构建与编译

### 构建命令

**使用 Arduino CLI 编译**:
```bash
arduino-cli compile --board esp32:esp32:esp32 --port COM3
```

**使用 Arduino CLI 上传**:
```bash
arduino-cli upload --board esp32:esp32:esp32 --port COM3 --upload-speed 921600
```

**串口监视器**:
```bash
arduino-cli monitor --port COM3 --baud-rate 115200
```

**清理构建缓存**:
```bash
rm -rf build/
```

### 编译配置
- 上传速度: 921600
- CPU频率: 240MHz
- 闪存频率: 80MHz
- 闪存模式: QIO
- 闪存大小: 4MB
- 分区方案: default

### 编译输出
- 输出目录: `build/`
- 编译产物: `build/sketch/*.cpp.o`

---

## 代码规范

### 基本格式
- **缩进**: 2 空格
- **行尾**: 保持一致（LF 或 CRLF）
- **文件编码**: UTF-8（支持中文注释）

### 命名规范
- **变量名**: 驼峰命名 (camelCase)
  - 示例: `serialBaud`, `displayWidth`, `currentPage`
- **常量/宏**: 全大写下划线分隔
  - 示例: `EPD2_COLOR`, `GxEPD_BLACK`, `UPLOAD_SPEED`
- **函数名**: 动词开头，驼峰命名
  - 示例: `initDisplay()`, `drawBitmap()`, `setRotation()`
- **类/类型名**: PascalCase
  - 示例: `GxEPD2_3C`, `FreeMonoBold9pt7b`
- **文件扩展名**:
  - 主程序: `.ino`
  - 头文件: `.h`
  - CPP源文件: `.cpp`

### 代码注释
- 始终使用简体中文
- 注释应解释"为什么"而非"是什么"
- 使用 `//` 进行单行注释
- 使用 `/* ... */` 进行多行注释或文件头说明

### 文件结构
```
项目目录/
├── *.ino          # 主程序文件
├── *.h            # 头文件
├── *.cpp          # 源文件
├── build/         # 编译输出目录
├── bitmaps/       # 位图资源
└── AGENTS.md      # 项目规则
```

### 头文件规范
- 使用 `#ifndef`/`#define`/`#endif` 防止重复包含
- 使用 `#pragma once` 可选（与前者二选一）
- 包含必要的系统头文件
- 使用 `const` 修饰只读数据

### 类型使用
- 显式指定变量类型，避免隐式类型转换
- 使用无符号类型处理位数据: `uint8_t`, `uint16_t`
- 指针使用显式类型: `const unsigned char*`

### 条件编译
- 使用 `#ifdef`/`#if defined()` 进行平台适配
- ESP32 系列使用 `CONFIG_IDF_TARGET_ESP32*` 宏区分芯片型号
- 为不同平台提供引脚配置注释

---

## 代码风格示例

### 正确示例
```cpp
// 墨水屏初始化
void initEpd() {
  display.init();
  display.setRotation(2);
  display.setFullWindow();
}

// 绘制位图
void drawImage(const unsigned char* imageData, int width, int height) {
  display.drawInvertedBitmap(0, 0, imageData, width, height, GxEPD_BLACK);
}
```

### 避免的做法
- ❌ 避免使用拼音命名: `pingmu` 应为 `display`
- ❌ 避免过长的单行代码
- ❌ 避免硬编码 Magic Numbers，应使用宏定义

---

## 错误处理

### 串口通信
- 使用 `Serial.begin(115200)` 初始化
- 错误信息通过 `Serial.println()` 输出
- 使用 `\n` 换行而非 `\r\n`（IDE自动处理）

### 显示初始化
- 检查 `init()` 返回值
- 使用 `display.nextPage()` / `display.firstPage()` 模式进行页面绘制
- 确保 BUSY 引脚正确检测忙碌状态

---

## 工作习惯

### 修改前
- 修改代码前先阅读相关文件
- 不确定时先问，不要猜测
- 每次只做最小必要的修改
- 理解现有代码的逻辑后再进行扩展

### 代码审查清单
- [ ] 所有新增代码使用中文注释
- [ ] 变量和函数命名符合规范
- [ ] 缩进使用 2 空格
- [ ] 条件编译正确处理平台差异
- [ ] 引脚配置与硬件连接一致
- [ ] 编译无错误和警告

### 硬件注意事项
- ESP32 GPIO 编号与开发板丝印可能不同
- 墨水屏需要正确的 SPI 引脚连接
- 上传前确保 COM 端口选择正确
- 三色屏需要 GxEPD2_3C 驱动

---

## 语言和风格

- 始终使用简体中文回复
- 直接回答问题，不要客套话
- 代码注释也用中文
- 保持回答简洁明了

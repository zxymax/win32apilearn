# Win32 API 编程教程与示例集合

## 项目简介

这是一个全面的Win32 API编程教程集合，通过10个递进式示例程序，帮助你循序渐进地掌握Windows桌面应用程序开发的核心概念和实践技巧。每个示例都经过精心设计，包含完整的Unicode中文支持，并配有详细的代码解释和学习指南。

## 学习路径

本教程按照从基础到进阶的顺序编排，建议按序号顺序学习：

1. **基础窗口（01_BasicWindow.cpp）** - 掌握窗口创建和消息循环的基本概念
2. **基本控件（02_BasicControls.cpp）** - 学习如何添加和操作UI控件
3. **GDI基础绘图（03_BasicGDI.cpp）** - 了解图形绘制和文本显示技术
4. **文件操作（04_FileOperations.cpp）** - 学习文件读写和信息获取
5. **菜单和对话框（05_MenuAndDialog.cpp）** - 掌握复杂UI交互元素的实现
6. **键盘输入处理（06_KeyboardInput.cpp）** - 学习如何处理键盘事件和输入
7. **鼠标输入处理（07_MouseHandling.cpp）** - 掌握鼠标事件处理和绘图功能
8. **定时器示例（08_TimerExample.cpp）** - 学习使用定时器实现动画和周期性任务
9. **资源管理（09_ResourceManagement.cpp）** - 了解Windows资源的加载和使用
10. **多线程编程（10_Multithreading.cpp）** - 掌握Windows多线程编程技术

## 快速入门指南

### 环境准备

1. **安装编译器**：推荐使用MinGW（Windows版GCC）或Visual Studio
2. **配置环境变量**：确保g++命令可以在命令行中直接调用

### 编译和运行示例程序

#### 单独编译和运行

使用以下命令单独编译和运行某个示例程序：

```powershell
# 编译示例1（基础窗口）
g++ 01_BasicWindow.cpp -o 01_BasicWindow.exe -mwindows

# 运行编译后的程序
01_BasicWindow.exe
```

#### 批量编译所有示例

使用以下命令批量编译所有示例程序：

```powershell
# 批处理命令（PowerShell）
foreach ($file in Get-ChildItem "0*.cpp") {
    g++ "$file" -o "$($file.BaseName).exe" -mwindows
    echo "编译完成: $($file.Name)"
}
```

或者在命令提示符（CMD）中使用：

```cmd
# 批处理命令（CMD）
for %i in (0*.cpp) do g++ %i -o %~ni.exe -mwindows && echo 编译完成: %i
```

## 示例程序详解

### 1. 基础窗口示例 (01_BasicWindow.cpp)

**学习目标**：理解Win32程序的基本架构和消息处理机制

**核心概念**：
- **窗口类**：Windows中所有窗口的基础，包含窗口的行为和外观定义
- **窗口过程**：处理所有发送到窗口的消息
- **消息循环**：从系统消息队列获取消息并分发

**关键代码解析**：

```cpp
// 定义UNICODE宏，确保支持中文显示
#define UNICODE
#define _UNICODE

// 窗口过程函数，处理各类消息
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT:  // 窗口需要重绘时的消息
            // 在这里处理绘制逻辑
            break;
        case WM_CLOSE:  // 窗口关闭时的消息
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:  // 窗口销毁时的消息
            PostQuitMessage(0);
            break;
        default:  // 处理未定义的消息
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
```

**练习建议**：
- 修改窗口标题和尺寸
- 添加更多消息处理（如鼠标点击、键盘输入）
- 尝试更改窗口样式和背景色

### 2. 基本控件示例 (02_BasicControls.cpp)

**学习目标**：掌握常见Windows控件的创建和使用

**支持的控件**：
- 按钮（Button）
- 编辑框（Edit）
- 静态文本（Static）
- 复选框（CheckBox）
- 单选按钮（RadioButton）

**关键代码解析**：

```cpp
// 在WM_CREATE消息中创建控件
case WM_CREATE:
    // 创建按钮控件
    CreateWindowW(
        L"BUTTON",       // 预定义的按钮类名
        L"点击我",      // 按钮文本
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // 样式
        10, 10, 100, 30,  // 位置和尺寸
        hwnd,             // 父窗口句柄
        (HMENU)ID_BUTTON, // 控件ID
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL
    );
    break;

// 处理控件通知消息
case WM_COMMAND:
    if (LOWORD(wParam) == ID_BUTTON && HIWORD(wParam) == BN_CLICKED) {
        // 按钮被点击时的处理逻辑
        MessageBoxW(hwnd, L"按钮被点击了！", L"提示", MB_OK);
    }
    break;
```

**练习建议**：
- 添加更多类型的控件
- 实现控件之间的数据交互
- 尝试设置不同的控件样式

### 3. GDI基础绘图示例 (03_BasicGDI.cpp)

**学习目标**：了解Windows图形设备接口（GDI）的基本绘图功能

**核心概念**：
- **设备上下文（DC）**：绘图的虚拟画布
- **GDI对象**：画笔、画刷、字体等绘图工具
- **文本绘制**：在窗口中显示文本

**关键代码解析**：

```cpp
case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        // 创建字体
        HFONT hFont = CreateFontW(
            24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_SWISS, L"微软雅黑"
        );
        
        // 选择字体到设备上下文
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        
        // 绘制中文文本
        TextOutW(hdc, 50, 50, L"GDI基础绘图示例 - 中文显示", wcslen(L"GDI基础绘图示例 - 中文显示"));
        
        // 恢复原来的字体并释放资源
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
        
        EndPaint(hwnd, &ps);
    }
    break;
```

**练习建议**：
- 尝试绘制不同形状（矩形、椭圆等）
- 使用不同颜色的画笔和画刷
- 实现简单的动画效果

### 4. 文件操作示例 (04_FileOperations.cpp)

**学习目标**：掌握Windows文件系统API的使用方法

**功能实现**：
- 文件创建和打开
- 文件读写操作
- 文件信息获取（大小、创建时间等）
- 错误处理机制

**关键代码解析**：

```cpp
// 创建和写入文件
HANDLE hFile = CreateFileW(
    L"sample.txt",                  // 文件名
    GENERIC_WRITE,                   // 访问权限
    0,                               // 共享模式
    NULL,                            // 安全属性
    CREATE_ALWAYS,                   // 创建方式
    FILE_ATTRIBUTE_NORMAL,           // 文件属性
    NULL                             // 模板文件
);

if (hFile != INVALID_HANDLE_VALUE) {
    DWORD dwBytesWritten;
    const WCHAR* lpBuffer = L"这是测试文本内容\n包含中文\n";
    BOOL bSuccess = WriteFile(
        hFile,                       // 文件句柄
        lpBuffer,                    // 数据缓冲区
        wcslen(lpBuffer) * sizeof(WCHAR), // 数据大小
        &dwBytesWritten,             // 实际写入字节数
        NULL                         // 重叠结构
    );
    CloseHandle(hFile);
}
```

**练习建议**：
- 实现文件复制和移动功能
- 创建一个简单的文本编辑器
- 尝试读取二进制文件

### 5. 菜单和对话框示例 (05_MenuAndDialog.cpp)

**学习目标**：掌握菜单和对话框的创建和使用

**功能实现**：
- 菜单的创建和管理
- 菜单命令的处理
- 对话框的实现

**关键代码解析**：

```cpp
// 创建菜单
HMENU hMenu = CreateMenu();
HMENU hFileMenu = CreatePopupMenu();
HMENU hHelpMenu = CreatePopupMenu();

// 添加菜单项
AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出(&X)");
AppendMenuW(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"关于(&A)");

// 将弹出菜单添加到主菜单
AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"文件(&F)");
AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"帮助(&H)");

// 设置窗口菜单
SetMenu(hwnd, hMenu);

// 处理菜单命令
case WM_COMMAND:
    switch (LOWORD(wParam)) {
        case ID_FILE_EXIT:
            DestroyWindow(hwnd);
            break;
        case ID_HELP_ABOUT:
            // 显示关于对话框
            MessageBoxW(hwnd, L"Win32示例程序\n版本 1.0", L"关于", MB_OK | MB_ICONINFORMATION);
            break;
    }
    break;
```

**练习建议**：
- 添加更多菜单项和子菜单
- 实现快捷键功能
- 设计更复杂的对话框

## Unicode中文显示完全指南

在Windows编程中，正确处理中文等非ASCII字符是非常重要的。本教程中的所有示例都已配置了完整的Unicode支持，具体实现方法如下：

### 1. 定义Unicode宏

在包含任何Windows头文件之前，必须定义以下宏：

```cpp
#define UNICODE
#define _UNICODE
```

这会告诉编译器使用宽字符版本的Windows API函数。

### 2. 使用宽字符API函数

确保使用带"W"后缀的宽字符版本函数：

```cpp
// 正确：使用宽字符版本
CreateWindowW(...);
MessageBoxW(...);
TextOutW(...);

// 错误：使用ASCII版本（会导致中文显示问题）
// CreateWindow(...);
// MessageBox(...);
// TextOut(...);
```

### 3. 使用宽字符类型

使用适当的宽字符数据类型：

```cpp
// 宽字符指针类型
LPCWSTR  // 常量宽字符指针
LPWSTR   // 可修改的宽字符指针

// 宽字符字符串
WCHAR szText[256];  // 替代char数组
```

### 4. 使用宽字符字符串

在字符串前添加"L"前缀表示宽字符：

```cpp
// 正确：宽字符字符串
L"中文文本";

// 错误：ASCII字符串
// "中文文本";
```

### 5. 字符串处理函数

使用宽字符版本的字符串处理函数：

```cpp
// 计算宽字符串长度
wcslen(L"中文文本");

// 格式化宽字符串
wsprintfW(szBuffer, L"格式化文本：%d", nValue);
```

### 6. 键盘输入处理示例 (06_KeyboardInput.cpp)

**学习目标**：掌握键盘输入事件的处理方法

**功能实现**：
- 键盘按下和释放事件处理
- 特殊键（如Shift、Ctrl、Alt）的检测
- 键盘快捷键的实现

**练习建议**：
- 实现文本输入功能
- 添加快捷键支持
- 创建一个简单的文本编辑器

### 7. 鼠标输入处理示例 (07_MouseHandling.cpp)

**学习目标**：掌握鼠标事件的处理和基本绘图功能

**功能实现**：
- 鼠标移动、点击事件处理
- 鼠标滚轮事件
- 拖动绘制功能
- 右键菜单实现

**练习建议**：
- 实现更复杂的绘图功能
- 添加颜色选择器
- 实现简单的图形编辑器

### 8. 定时器示例 (08_TimerExample.cpp)

**学习目标**：学习使用定时器实现动画和周期性任务

**功能实现**：
- 定时器的创建和管理
- 动画效果实现
- 周期性任务调度

**练习建议**：
- 创建更复杂的动画效果
- 实现游戏循环
- 添加时钟或倒计时功能

### 9. 资源管理示例 (09_ResourceManagement.cpp)

**学习目标**：了解Windows资源的加载和使用

**功能实现**：
- 图标和光标资源的加载
- 位图和图像资源的使用
- 资源的释放和管理

**练习建议**：
- 添加自定义图标
- 实现图像浏览器
- 学习资源文件(.rc)的创建

### 10. 多线程编程示例 (10_Multithreading.cpp)

**学习目标**：掌握Windows多线程编程技术

**功能实现**：
- 线程的创建和管理
- 线程同步机制
- 多线程间的通信
- 避免竞态条件

**练习建议**：
- 实现生产者-消费者模式
- 创建多线程下载器
- 学习线程池的实现

## 常见问题与解决方案

### 编译错误

- **问题**：找不到头文件或函数定义
  **解决**：确保正确安装了MinGW，并配置了环境变量

- **问题**：链接错误（undefined reference）
  **解决**：添加必要的库文件，如`-lgdi32`、`-lcomdlg32`等

### 运行时错误

- **问题**：程序崩溃或无响应
  **解决**：检查指针使用、资源释放和消息处理逻辑

- **问题**：中文显示为乱码
  **解决**：确保正确配置了Unicode支持，按照前面的指南检查代码

- **问题**：线程相关错误
  **解决**：检查线程同步和资源共享，确保线程安全

## 进阶学习资源

- **官方文档**：[Windows API文档](https://docs.microsoft.com/en-us/windows/win32/api/)
- **推荐书籍**：
  - 《Windows程序设计》（Charles Petzold）
  - 《Windows核心编程》（Jeffrey Richter）
  - 《Windows多线程编程实战》
- **在线教程**：Microsoft Learn平台的Win32开发课程

## 项目维护

### 编译注意事项

- 所有示例程序都应使用`-mwindows`选项进行编译，以创建窗口应用程序
- 对于多线程程序，可能需要添加`-lpthread`或其他相关库

### 文件组织

- 源文件：以数字开头的.cpp文件，按功能和难度递增排序
- 配置文件：README.md包含完整的项目说明和使用指南

## 贡献与反馈

如果你发现代码中的问题或有改进建议，欢迎提出反馈。希望这个教程能帮助你入门Win32 API编程！

## 许可证

本项目采用MIT许可证，欢迎自由使用和修改。
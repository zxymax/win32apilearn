// 01_BasicWindow.cpp
// 教学内容：基础Win32窗口创建
// 教学大纲：
// 1. Win32 API的基本结构
// 2. 窗口类的注册
// 3. 窗口的创建和显示
// 4. 消息循环的实现
// 5. 窗口过程函数的编写

// 定义使用Unicode字符集（必须在包含windows.h之前定义）
#define UNICODE
#define _UNICODE

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"BasicWin32Window";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32基础窗口示例";  // 窗口标题（宽字符版本）

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明

// WinMain函数 - Win32应用程序的入口点
// 参数说明：
// hInstance: 当前应用程序实例的句柄
// hPrevInstance: 先前应用程序实例的句柄，在32位和64位Windows中始终为NULL
// lpCmdLine: 命令行参数字符串
// nCmdShow: 窗口的显示方式（最大化、最小化、正常等）
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 窗口类结构体定义
    WNDCLASSEX wcex;

    // 填充窗口类结构体的各个成员
    wcex.cbSize = sizeof(WNDCLASSEX);  // 结构体大小
    wcex.style = CS_HREDRAW | CS_VREDRAW;  // 窗口类样式：水平和垂直重绘
    wcex.lpfnWndProc = WndProc;  // 窗口过程函数指针
    wcex.cbClsExtra = 0;  // 类额外字节数
    wcex.cbWndExtra = 0;  // 窗口额外字节数
    wcex.hInstance = hInstance;  // 应用程序实例句柄
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // 加载默认应用程序图标
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);  // 加载默认箭头光标
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 设置窗口背景色
    wcex.lpszMenuName = NULL;  // 菜单名称（无菜单）
    wcex.lpszClassName = szWindowClass;  // 窗口类名
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);  // 加载小图标

    // 注册窗口类
    if (!RegisterClassEx(&wcex))
    {
        // 注册失败时显示错误消息（使用宽字符版本）
        MessageBoxW(NULL, L"窗口类注册失败！", L"错误", MB_ICONERROR);
        return 1;  // 返回错误代码
    }

    // 保存应用程序实例句柄
    hInst = hInstance;

    // 创建窗口（使用宽字符版本）
    HWND hWnd = CreateWindowW(
        szWindowClass,  // 窗口类名
        szTitle,  // 窗口标题
        WS_OVERLAPPEDWINDOW,  // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,  // 窗口位置（默认）
        500, 300,  // 窗口宽度和高度
        NULL,  // 父窗口句柄（无父窗口）
        NULL,  // 菜单句柄（无菜单）
        hInstance,  // 应用程序实例句柄
        NULL  // 创建参数（无）
    );

    // 检查窗口是否创建成功
    if (!hWnd)
    {
        // 创建失败时显示错误消息（使用宽字符版本）
        MessageBoxW(NULL, L"窗口创建失败！", L"错误", MB_ICONERROR);
        return 1;  // 返回错误代码
    }

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    // 更新窗口
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    // 从消息队列中获取消息，直到接收到WM_QUIT消息
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // 翻译虚拟键消息为字符消息
        TranslateMessage(&msg);
        // 将消息分派到相应的窗口过程函数
        DispatchMessage(&msg);
    }

    // 返回退出代码
    return (int)msg.wParam;
}

// 窗口过程函数 - 处理发送到窗口的所有消息
// 参数说明：
// hWnd: 接收消息的窗口句柄
// message: 消息标识符
// wParam: 消息的附加参数（字参数）
// lParam: 消息的附加参数（长字参数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 根据消息类型执行不同的处理
    switch (message)
    {
        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;  // 绘图结构体
            HDC hdc = BeginPaint(hWnd, &ps);  // 开始绘图，获取设备上下文

            // 定义要绘制的文本（使用宽字符）
            LPCWSTR text = L"Hello, Win32 API! 这是一个基础窗口示例。";
            // 在窗口客户区绘制文本（使用宽字符版本的函数）
            TextOutW(hdc, 50, 50, text, wcslen(text));

            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;
        case WM_DESTROY:  // 窗口销毁消息
            PostQuitMessage(0);  // 向应用程序发送退出消息
            break;
        default:
            // 处理未定义的消息
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;  // 返回处理结果
}

// 总结：
// 1. WinMain函数：Win32应用程序的入口点，负责初始化应用程序、注册窗口类、创建窗口和启动消息循环。
// 2. WNDCLASSEX结构体：定义窗口类的属性，包括窗口样式、窗口过程函数、图标、光标等。
// 3. RegisterClassEx函数：注册窗口类，使操作系统知道如何创建该类型的窗口。
// 4. CreateWindow函数：创建窗口，返回窗口句柄。
// 5. ShowWindow和UpdateWindow函数：显示和更新窗口。
// 6. 消息循环：通过GetMessage、TranslateMessage和DispatchMessage函数处理消息。
// 7. WndProc函数：窗口过程函数，处理发送到窗口的各种消息。
// 8. 常用消息：WM_PAINT（重绘）、WM_DESTROY（销毁）等。
// 注意事项：
//    - 必须正确注册窗口类后才能创建窗口
//    - 消息循环是Win32应用程序的核心，负责处理用户输入和系统事件
//    - 窗口过程函数必须返回DefWindowProc处理未定义的消息
//    - 明确使用Unicode字符集和宽字符函数可以确保中文等非ASCII字符正确显示
//    - 使用wcslen替代lstrlen处理宽字符串长度计算
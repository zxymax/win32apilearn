// 02_BasicControls.cpp
// 教学内容：Win32基础控件的创建和使用
// 教学大纲：
// 1. 按钮控件的创建和消息处理
// 2. 静态文本控件的创建
// 3. 编辑框控件的创建和文本获取
// 4. 控件ID的定义和使用
// 5. 对话框消息的处理

// 定义使用Unicode字符集（必须在包含windows.h之前定义）
#define UNICODE
#define _UNICODE

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"BasicControlsWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32基础控件示例";  // 窗口标题（宽字符版本）

// 控件ID定义 - 用于标识不同的控件
#define ID_BUTTON 1001  // 按钮控件ID
#define ID_EDIT   1002  // 编辑框控件ID
#define ID_STATIC 1003  // 静态文本控件ID

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明

// WinMain函数 - Win32应用程序的入口点
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
                MessageBoxW(NULL, L"窗口类注册失败！", L"错误", MB_ICONERROR);
                return 1;
            }

    // 保存应用程序实例句柄
    hInst = hInstance;

    // 创建窗口
    HWND hWnd = CreateWindow(
        szWindowClass,  // 窗口类名
        szTitle,  // 窗口标题
        WS_OVERLAPPEDWINDOW,  // 窗口样式
        CW_USEDEFAULT, CW_USEDEFAULT,  // 窗口位置
        500, 350,  // 窗口宽度和高度
        NULL,  // 父窗口句柄
        NULL,  // 菜单句柄
        hInstance,  // 应用程序实例句柄
        NULL  // 创建参数
    );

    // 检查窗口是否创建成功
            if (!hWnd)
            {
                MessageBoxW(NULL, L"窗口创建失败！", L"错误", MB_ICONERROR);
                return 1;
            }

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);
    // 更新窗口
    UpdateWindow(hWnd);

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// 窗口过程函数 - 处理发送到窗口的所有消息
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 根据消息类型执行不同的处理
    switch (message)
    {
        case WM_CREATE:  // 窗口创建时发送的消息
        {
            // 创建静态文本控件
            // 参数：控件类名、文本内容、控件样式、位置、大小、父窗口句柄、控件ID、实例句柄、附加数据
            CreateWindowW(
                L"STATIC",  // 静态文本控件的类名（系统预定义，宽字符版本）
                L"请输入您的名字：",  // 控件显示的文本（宽字符版本）
                WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE,  // 控件样式：可见、子窗口、文本居中
                20, 50, 120, 25,  // 控件位置(x, y)和大小(width, height)
                hWnd,  // 父窗口句柄
                (HMENU)ID_STATIC,  // 控件ID
                hInst,  // 应用程序实例句柄
                NULL  // 附加数据
            );

            // 创建编辑框控件
            CreateWindowW(
                L"EDIT",  // 编辑框控件的类名（系统预定义，宽字符版本）
                L"",  // 默认文本为空（宽字符版本）
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,  // 控件样式：可见、子窗口、带边框、左对齐
                150, 50, 200, 25,  // 控件位置和大小
                hWnd,  // 父窗口句柄
                (HMENU)ID_EDIT,  // 控件ID
                hInst,  // 应用程序实例句柄
                NULL  // 附加数据
            );

            // 创建按钮控件
            CreateWindowW(
                L"BUTTON",  // 按钮控件的类名（系统预定义，宽字符版本）
                L"显示欢迎消息",  // 按钮上的文本（宽字符版本）
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // 控件样式：可见、子窗口、按钮样式
                150, 100, 150, 30,  // 控件位置和大小
                hWnd,  // 父窗口句柄
                (HMENU)ID_BUTTON,  // 控件ID
                hInst,  // 应用程序实例句柄
                NULL  // 附加数据
            );
        }
        break;

        case WM_COMMAND:  // 命令消息，通常由控件发送
        {
            // 获取发送命令的控件ID
            int wmId = LOWORD(wParam);
            // 获取通知代码
            int wmEvent = HIWORD(wParam);

            // 根据控件ID和通知代码处理命令
            switch (wmId)
            {
                case ID_BUTTON:  // 如果是按钮被点击
                    if (wmEvent == BN_CLICKED)  // 按钮点击通知
                    {
                        // 定义缓冲区用于存储编辑框中的文本
                        WCHAR szBuffer[100];
                        // 获取编辑框控件的句柄
                        HWND hEdit = GetDlgItem(hWnd, ID_EDIT);
                        // 从编辑框中获取文本（使用宽字符版本的函数）
                        GetWindowTextW(hEdit, szBuffer, ARRAYSIZE(szBuffer));

                        // 定义欢迎消息字符串
                        WCHAR szMessage[200];
                        // 格式化欢迎消息（使用宽字符版本的函数）
                        wsprintfW(szMessage, L"你好，%s！欢迎使用Win32控件示例。", szBuffer);
                        
                        // 显示欢迎消息对话框（使用宽字符版本的函数）
                        MessageBoxW(hWnd, szMessage, L"欢迎", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                default:
                    // 处理未定义的命令
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // 在窗口顶部绘制标题文本
            LPCWSTR titleText = L"Win32控件示例程序";
            // 设置文本颜色为蓝色
            SetTextColor(hdc, RGB(0, 0, 255));
            // 设置文本背景模式为透明
            SetBkMode(hdc, TRANSPARENT);
            // 绘制文本（使用宽字符版本的函数）
            TextOutW(hdc, 150, 20, titleText, wcslen(titleText));

            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:  // 窗口销毁消息
            PostQuitMessage(0);  // 退出应用程序
            break;

        default:
            // 处理未定义的消息
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 总结：
// 1. 控件ID的定义：使用#define预处理器指令定义唯一的控件ID，用于标识不同的控件。
// 2. 常用控件的创建：
//    - 静态文本控件(STATIC)：用于显示不可编辑的文本
//    - 编辑框控件(EDIT)：用于接收用户输入的文本
//    - 按钮控件(BUTTON)：用于触发操作
// 3. 控件样式：通过组合不同的样式标志来设置控件的外观和行为。
// 4. 命令消息处理：
//    - WM_COMMAND消息：用于处理来自控件的命令
//    - LOWORD(wParam)：获取发送命令的控件ID
//    - HIWORD(wParam)：获取通知代码（如BN_CLICKED表示按钮被点击）
// 5. 控件操作函数：
//    - GetDlgItem：通过ID获取控件句柄
//    - GetWindowText：获取控件的文本内容
//    - wsprintf：格式化字符串
// 6. 文本绘制函数：
//    - SetTextColor：设置文本颜色
//    - SetBkMode：设置文本背景模式
// 7. 注意事项：
//    - 控件必须在WM_CREATE消息处理中创建，确保窗口已经存在
//    - 控件必须设置WS_CHILD和WS_VISIBLE样式才能正确显示和接收消息
//    - 父窗口句柄必须设置为当前窗口的句柄
//    - 每个控件必须有唯一的ID，以便在消息处理中识别
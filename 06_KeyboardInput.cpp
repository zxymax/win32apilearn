// 06_KeyboardInput.cpp
// 教学内容：Win32键盘输入处理
// 教学大纲：
// 1. 键盘消息的类型和处理
// 2. 按键状态的检测
// 3. 字符输入的处理
// 4. 特殊按键的识别
// 5. 键盘加速键的实现

// 定义使用Unicode字符集（必须在包含windows.h之前定义）
#define UNICODE
#define _UNICODE

// 定义鼠标位置获取宏（在某些Windows SDK版本中可能未定义）
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"KeyboardInputWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32键盘输入处理示例";  // 窗口标题（宽字符版本）

// 按键信息结构体
struct KeyInfo {
    WCHAR lastKey;      // 最后按下的字符键
    WCHAR specialKey[50]; // 最后按下的特殊键
    BOOL shiftPressed;  // Shift键是否按下
    BOOL ctrlPressed;   // Ctrl键是否按下
    BOOL altPressed;    // Alt键是否按下
};

// 全局按键信息
KeyInfo g_keyInfo = {0, L"", FALSE, FALSE, FALSE};

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
void UpdateKeyStatus(HWND hWnd);  // 更新按键状态显示

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
        600, 400,  // 窗口宽度和高度
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 根据消息类型执行不同的处理
    switch (message)
    {
        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;  // 绘图结构体
            HDC hdc = BeginPaint(hWnd, &ps);  // 开始绘图，获取设备上下文

            // 设置文本颜色和背景模式
            SetTextColor(hdc, RGB(0, 0, 128));
            SetBkMode(hdc, TRANSPARENT);

            // 创建字体
            HFONT hFont = CreateFontW(
                16,  // 字体高度
                0,   // 字体宽度（0表示自动调整）
                0,   // 文本倾斜角度
                0,   // 文本方向角度
                FW_NORMAL,  // 字体粗细
                FALSE,  // 是否斜体
                FALSE,  // 是否下划线
                FALSE,  // 是否删除线
                DEFAULT_CHARSET,  // 字符集
                OUT_DEFAULT_PRECIS,  // 输出精度
                CLIP_DEFAULT_PRECIS,  // 裁剪精度
                DEFAULT_QUALITY,  // 输出质量
                DEFAULT_PITCH | FF_SWISS,  // 字体间距和族
                L"Arial"  // 字体名称（宽字符版本）
            );

            // 选择字体
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            // 绘制说明文本
            LPCWSTR instructions = L"请按键以测试键盘输入处理功能。\n支持以下功能：\n1. 字符键输入\n2. 特殊键（F1-F12、方向键等）识别\n3. Shift/Ctrl/Alt组合键检测\n4. F1键显示帮助信息\n5. Esc键清空输入历史";
            RECT rect = {50, 30, 550, 150};
            DrawTextW(hdc, instructions, -1, &rect, DT_LEFT | DT_WORDBREAK);

            // 绘制按键状态
            WCHAR keyStatus[500];
            wsprintfW(keyStatus, L"最后按下的字符键: %c\n最后按下的特殊键: %s\n修饰键状态:\n- Shift: %s\n- Ctrl: %s\n- Alt: %s", 
                g_keyInfo.lastKey ? g_keyInfo.lastKey : L'无', 
                g_keyInfo.specialKey[0] ? g_keyInfo.specialKey : L"无", 
                g_keyInfo.shiftPressed ? L"按下" : L"未按下", 
                g_keyInfo.ctrlPressed ? L"按下" : L"未按下", 
                g_keyInfo.altPressed ? L"按下" : L"未按下");
            RECT statusRect = {50, 180, 550, 350};
            DrawTextW(hdc, keyStatus, -1, &statusRect, DT_LEFT | DT_WORDBREAK);

            // 清理资源
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;

        case WM_KEYDOWN:  // 按键按下消息
        {
            // 获取按键的虚拟键码
            WPARAM vKey = wParam;
            
            // 更新修饰键状态
            g_keyInfo.shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            g_keyInfo.ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            g_keyInfo.altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;
            
            // 处理特殊键
            switch (vKey)
            {
                case VK_ESCAPE:  // Esc键
                    // 清空输入历史
                    g_keyInfo.lastKey = 0;
                    g_keyInfo.specialKey[0] = L'\0';
                    g_keyInfo.shiftPressed = FALSE;
                    g_keyInfo.ctrlPressed = FALSE;
                    g_keyInfo.altPressed = FALSE;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_F1:  // F1键 - 显示帮助
                    MessageBoxW(hWnd, L"键盘输入处理帮助\n\n按键功能:\n- 字母、数字键: 记录最后按下的字符\n- 方向键、功能键等: 记录特殊键信息\n- Esc: 清空输入历史\n- F1: 显示此帮助信息", 
                        L"帮助", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case VK_LEFT:  // 左方向键
                    wcscpy_s(g_keyInfo.specialKey, L"左方向键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_RIGHT:  // 右方向键
                    wcscpy_s(g_keyInfo.specialKey, L"右方向键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_UP:  // 上方向键
                    wcscpy_s(g_keyInfo.specialKey, L"上方向键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_DOWN:  // 下方向键
                    wcscpy_s(g_keyInfo.specialKey, L"下方向键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_SPACE:  // 空格键
                    wcscpy_s(g_keyInfo.specialKey, L"空格键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case VK_TAB:  // Tab键
                    wcscpy_s(g_keyInfo.specialKey, L"Tab键");
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                default:
                    // 对于F2-F12等功能键
                    if (vKey >= VK_F2 && vKey <= VK_F12)
                    {
                        wsprintfW(g_keyInfo.specialKey, L"F%d键", vKey - VK_F1 + 1);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    break;
            }
        }
        break;

        case WM_CHAR:  // 字符输入消息
        {
            // 获取输入的字符
            WCHAR ch = (WCHAR)wParam;
            
            // 更新最后按下的字符键
            g_keyInfo.lastKey = ch;
            
            // 重绘窗口以显示更新后的按键状态
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_KEYUP:  // 按键释放消息
        {
            // 更新修饰键状态
            g_keyInfo.shiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            g_keyInfo.ctrlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            g_keyInfo.altPressed = (GetKeyState(VK_MENU) & 0x8000) != 0;
            
            // 重绘窗口以显示更新后的按键状态
            InvalidateRect(hWnd, NULL, TRUE);
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

// 更新按键状态显示
void UpdateKeyStatus(HWND hWnd)
{
    // 使窗口客户区无效，触发WM_PAINT消息
    InvalidateRect(hWnd, NULL, TRUE);
}

// 总结：
// 1. 键盘输入处理主要通过三种消息：WM_KEYDOWN（按键按下）、WM_KEYUP（按键释放）和WM_CHAR（字符输入）
// 2. WM_KEYDOWN/WM_KEYUP处理虚拟键码，可以识别特殊键和功能键
// 3. WM_CHAR处理实际的字符输入，已经考虑了Shift键的状态和键盘布局
// 4. GetKeyState函数可以获取修饰键（Shift、Ctrl、Alt）的当前状态
// 5. 常见的虚拟键码包括：VK_ESCAPE、VK_SPACE、VK_RETURN、VK_TAB、VK_LEFT/RIGHT/UP/DOWN、VK_F1-F12等
// 6. 处理键盘输入时，需要考虑键的重复、组合键等情况
// 7. 对于需要实时响应键盘输入的应用（如游戏），可以使用GetAsyncKeyState函数获取异步键盘状态
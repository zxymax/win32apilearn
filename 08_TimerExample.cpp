// 08_TimerExample.cpp
// 教学内容：Win32定时器的使用
// 教学大纲：
// 1. 定时器的创建和销毁
// 2. 定时器消息的处理
// 3. 多个定时器的管理
// 4. 定时器精度和性能考虑
// 5. 定时器的实际应用示例

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
LPCWSTR szWindowClass = L"TimerExampleWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32定时器示例";  // 窗口标题（宽字符版本）

// 定时器ID定义
#define TIMER_ID_1 101  // 第一个定时器ID
#define TIMER_ID_2 102  // 第二个定时器ID
#define TIMER_ID_3 103  // 第三个定时器ID

// 定时器信息结构体
typedef struct {
    UINT_PTR id;         // 定时器ID
    UINT interval;       // 定时间隔（毫秒）
    BOOL isRunning;      // 是否正在运行
    DWORD count;         // 触发次数
    DWORD startTime;     // 启动时间
} TimerInfo;

// 全局定时器信息数组
TimerInfo g_timers[] = {
    {TIMER_ID_1, 1000, FALSE, 0, 0},  // 1秒定时器
    {TIMER_ID_2, 500, FALSE, 0, 0},   // 0.5秒定时器
    {TIMER_ID_3, 100, FALSE, 0, 0}    // 0.1秒定时器
};

// 动画相关变量
int g_animationPosition = 0;  // 动画位置
int g_animationDirection = 1; // 动画方向（1为右，-1为左）

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
void DrawTimerInfo(HDC hdc, RECT* prc);  // 绘制定时器信息
void DrawAnimation(HDC hdc, RECT* prc);  // 绘制动画

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
        600, 500,  // 窗口宽度和高度
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
        case WM_CREATE:  // 窗口创建消息
        {
            // 创建控制按钮
            CreateWindowW(
                L"BUTTON", L"启动所有定时器", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                50, 400, 150, 30, hWnd, (HMENU)1001, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"停止所有定时器", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                220, 400, 150, 30, hWnd, (HMENU)1002, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"重置计数器", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                390, 400, 150, 30, hWnd, (HMENU)1003, hInst, NULL
            );
        }
        break;

        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;  // 绘图结构体
            HDC hdc = BeginPaint(hWnd, &ps);  // 开始绘图，获取设备上下文

            // 获取客户区矩形
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            // 设置文本颜色和背景模式
            SetTextColor(hdc, RGB(0, 0, 128));
            SetBkMode(hdc, TRANSPARENT);

            // 绘制说明文本
            LPCWSTR instructions = L"定时器示例演示\n\n操作说明：\n1. 点击按钮控制定时器\n2. 观察三个不同频率的定时器工作\n3. 底部动画由定时器驱动\n4. 定时器精度受系统调度影响";
            RECT rect = {50, 30, 550, 120};
            DrawTextW(hdc, instructions, -1, &rect, DT_LEFT | DT_WORDBREAK);

            // 绘制定时器信息
            DrawTimerInfo(hdc, &rcClient);

            // 绘制动画
            DrawAnimation(hdc, &rcClient);

            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;

        case WM_TIMER:  // 定时器消息
        {
            // 获取定时器ID
            UINT_PTR timerId = wParam;
            
            // 查找对应的定时器
            for (int i = 0; i < ARRAYSIZE(g_timers); i++)
            {
                if (g_timers[i].id == timerId)
                {
                    // 增加触发次数
                    g_timers[i].count++;
                    
                    // 如果是最快的定时器，更新动画位置
                    if (timerId == TIMER_ID_3)
                    {
                        g_animationPosition += g_animationDirection;
                        
                        // 检查边界，反转方向
                        if (g_animationPosition >= 500 || g_animationPosition <= 50)
                        {
                            g_animationDirection *= -1;
                        }
                    }
                    
                    // 重绘窗口，确保清除背景
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                }
            }
        }
        break;

        case WM_COMMAND:  // 命令消息
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
                case 1001:  // 启动所有定时器
                    for (int i = 0; i < ARRAYSIZE(g_timers); i++)
                    {
                        if (!g_timers[i].isRunning)
                        {
                            // 创建定时器
                            SetTimer(hWnd, g_timers[i].id, g_timers[i].interval, NULL);
                            g_timers[i].isRunning = TRUE;
                            g_timers[i].startTime = GetTickCount();
                        }
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case 1002:  // 停止所有定时器
                    for (int i = 0; i < ARRAYSIZE(g_timers); i++)
                    {
                        if (g_timers[i].isRunning)
                        {
                            // 销毁定时器
                            KillTimer(hWnd, g_timers[i].id);
                            g_timers[i].isRunning = FALSE;
                        }
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case 1003:  // 重置计数器
                    for (int i = 0; i < ARRAYSIZE(g_timers); i++)
                    {
                        g_timers[i].count = 0;
                        g_timers[i].startTime = GetTickCount();
                    }
                    g_animationPosition = 50;
                    g_animationDirection = 1;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
        }
        break;

        case WM_DESTROY:  // 窗口销毁消息
        {
            // 确保所有定时器都被销毁
            for (int i = 0; i < ARRAYSIZE(g_timers); i++)
            {
                if (g_timers[i].isRunning)
                {
                    KillTimer(hWnd, g_timers[i].id);
                }
            }
            
            PostQuitMessage(0);  // 向应用程序发送退出消息
        }
        break;

        default:
            // 处理未定义的消息
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;  // 返回处理结果
}

// 绘制定时器信息
void DrawTimerInfo(HDC hdc, RECT* prc)
{
    // 创建字体
    HFONT hFont = CreateFontW(
        14,  // 字体高度
        0,   // 字体宽度
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
        L"Arial"  // 字体名称
    );
    
    // 选择字体
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    // 绘制表头
    RECT headerRect = {50, 140, 550, 160};
    DrawTextW(hdc, L"定时器信息:", -1, &headerRect, DT_LEFT);
    
    // 绘制分隔线
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 50, 165, NULL);
    LineTo(hdc, 550, 165);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    
    // 绘制表头行
    WCHAR header[200];
    wsprintfW(header, L"定时器ID    间隔(ms)    状态    触发次数    运行时间(s)");
    RECT headerRowRect = {50, 170, 550, 190};
    DrawTextW(hdc, header, -1, &headerRowRect, DT_LEFT);
    
    // 绘制每个定时器的信息
    for (int i = 0; i < ARRAYSIZE(g_timers); i++)
    {
        WCHAR info[200];
        DWORD elapsedTime = 0;
        
        if (g_timers[i].isRunning)
        {
            elapsedTime = (GetTickCount() - g_timers[i].startTime) / 1000;
        }
        
        wsprintfW(info, L"%-12d %-12d %-8s %-12d %-12d",
            g_timers[i].id,
            g_timers[i].interval,
            g_timers[i].isRunning ? L"运行中" : L"已停止",
            g_timers[i].count,
            elapsedTime
        );
        
        RECT infoRect = {50, 190 + i * 25, 550, 215 + i * 25};
        DrawTextW(hdc, info, -1, &infoRect, DT_LEFT);
    }
    
    // 清理资源
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// 绘制动画
void DrawAnimation(HDC hdc, RECT* prc)
{
    // 创建动画对象的画笔和画刷
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 192, 192));
    
    // 选择GDI对象
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    // 绘制动画矩形
    Rectangle(hdc, g_animationPosition, 320, g_animationPosition + 50, 370);
    
    // 绘制动画标题
    WCHAR title[50];
    wsprintfW(title, L"动画");
    RECT titleRect = {g_animationPosition + 10, 335, g_animationPosition + 40, 355};
    DrawTextW(hdc, title, -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    // 清理资源
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// 总结：
// 1. 定时器的创建和使用：
//    - 使用SetTimer函数创建定时器，指定定时器ID、间隔时间和回调函数
//    - 定时器消息通过WM_TIMER发送到窗口过程函数
//    - 使用KillTimer函数销毁定时器
// 2. 定时器的特点：
//    - 精度有限，通常为10-55毫秒，受系统调度影响
//    - 当系统忙碌时，定时器消息可能会被合并或延迟
//    - 适合不需要高精度的定时任务
// 3. 定时器的应用场景：
//    - 简单动画效果
//    - 定期更新界面信息
//    - 定时执行后台任务
//    - 模拟时钟或倒计时器
// 4. 注意事项：
//    - 每个窗口可以有多个不同ID的定时器
//    - 定时器回调函数中不应执行耗时操作，以免阻塞消息循环
//    - 应用程序退出前应该销毁所有定时器
//    - 对于高精度定时需求，可以考虑使用多媒体定时器或线程
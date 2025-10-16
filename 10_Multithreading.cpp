// 10_Multithreading.cpp
// 教学内容：Win32多线程编程
// 教学大纲：
// 1. 线程的创建和终止
// 2. 线程同步机制
// 3. 临界区的使用
// 4. 事件对象的使用
// 5. 线程池的概念
// 6. 多线程编程的最佳实践

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
#include <vector>

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"MultithreadingWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32多线程编程示例";  // 窗口标题（宽字符版本）

// 线程同步对象
CRITICAL_SECTION g_criticalSection;  // 临界区对象
HANDLE g_eventStart;  // 开始事件
HANDLE g_eventStop;   // 停止事件

// 线程状态信息
struct ThreadInfo {
    HANDLE hThread;     // 线程句柄
    DWORD dwThreadId;   // 线程ID
    int threadNum;      // 线程编号
    int counter;        // 计数器
    BOOL isRunning;     // 是否运行中
};

// 全局线程信息数组
std::vector<ThreadInfo> g_threads;

// 控制变量
int g_threadCount = 0;  // 当前线程数量
int g_totalCounter = 0; // 总计数器
BOOL g_threadsRunning = FALSE; // 线程是否正在运行

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
DWORD WINAPI ThreadProc(LPVOID lpParameter);  // 线程函数
void UpdateThreadInfo(HWND hWnd);  // 更新线程信息显示
void CreateThreads(HWND hWnd, int count);  // 创建线程
void StopAllThreads();  // 停止所有线程
void DrawThreadInfo(HDC hdc, RECT* prc);  // 绘制线程信息

// WinMain函数 - Win32应用程序的入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 初始化临界区
    InitializeCriticalSection(&g_criticalSection);
    
    // 创建事件对象
    g_eventStart = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_eventStop = CreateEvent(NULL, TRUE, TRUE, NULL);
    
    if (!g_eventStart || !g_eventStop)
    {
        MessageBoxW(NULL, L"创建事件对象失败！", L"错误", MB_ICONERROR);
        DeleteCriticalSection(&g_criticalSection);
        return 1;
    }

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
        DeleteCriticalSection(&g_criticalSection);
        CloseHandle(g_eventStart);
        CloseHandle(g_eventStop);
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
        700, 500,  // 窗口宽度和高度
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
        DeleteCriticalSection(&g_criticalSection);
        CloseHandle(g_eventStart);
        CloseHandle(g_eventStop);
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

    // 清理资源
    StopAllThreads();
    DeleteCriticalSection(&g_criticalSection);
    CloseHandle(g_eventStart);
    CloseHandle(g_eventStop);

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
                L"BUTTON", L"创建2个线程", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                50, 420, 120, 30, hWnd, (HMENU)1001, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"创建4个线程", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                180, 420, 120, 30, hWnd, (HMENU)1002, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"开始所有线程", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                310, 420, 120, 30, hWnd, (HMENU)1003, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"停止所有线程", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                440, 420, 120, 30, hWnd, (HMENU)1004, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"清除所有线程", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                570, 420, 100, 30, hWnd, (HMENU)1005, hInst, NULL
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
            LPCWSTR instructions = L"多线程编程示例演示\n\n操作说明：\n1. 创建线程：创建指定数量的工作线程\n2. 开始所有线程：启动所有已创建的线程执行计算任务\n3. 停止所有线程：暂停所有线程的执行\n4. 清除所有线程：终止并清理所有线程\n\n本示例展示了线程同步、临界区保护和事件对象的使用";
            RECT rect = {50, 30, 650, 150};
            DrawTextW(hdc, instructions, -1, &rect, DT_LEFT | DT_WORDBREAK);

            // 绘制线程信息
            DrawThreadInfo(hdc, &rcClient);

            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;

        case WM_COMMAND:  // 命令消息
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
                case 1001:  // 创建2个线程
                    if (!g_threadsRunning)
                    {
                        CreateThreads(hWnd, 2);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"请先停止当前运行的线程！", L"提示", MB_OK | MB_ICONWARNING);
                    }
                    break;
                    
                case 1002:  // 创建4个线程
                    if (!g_threadsRunning)
                    {
                        CreateThreads(hWnd, 4);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"请先停止当前运行的线程！", L"提示", MB_OK | MB_ICONWARNING);
                    }
                    break;
                    
                case 1003:  // 开始所有线程
                    if (!g_threads.empty() && !g_threadsRunning)
                    {
                        g_threadsRunning = TRUE;
                        
                        // 重置事件状态
                        ResetEvent(g_eventStop);
                        SetEvent(g_eventStart);
                        
                        MessageBoxW(hWnd, L"所有线程已开始运行！", L"成功", MB_OK | MB_ICONINFORMATION);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else if (g_threads.empty())
                    {
                        MessageBoxW(hWnd, L"请先创建线程！", L"提示", MB_OK | MB_ICONWARNING);
                    }
                    break;
                    
                case 1004:  // 停止所有线程
                    if (g_threadsRunning)
                    {
                        StopAllThreads();
                        MessageBoxW(hWnd, L"所有线程已停止！", L"成功", MB_OK | MB_ICONINFORMATION);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"线程当前未运行！", L"提示", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                    
                case 1005:  // 清除所有线程
                    if (!g_threadsRunning)
                    {
                        StopAllThreads();
                        
                        // 关闭并清除所有线程句柄
                        for (size_t i = 0; i < g_threads.size(); i++)
                        {
                            if (g_threads[i].hThread)
                            {
                                CloseHandle(g_threads[i].hThread);
                                g_threads[i].hThread = NULL;
                            }
                        }
                        
                        g_threads.clear();
                        g_threadCount = 0;
                        g_totalCounter = 0;
                        
                        MessageBoxW(hWnd, L"所有线程已清除！", L"成功", MB_OK | MB_ICONINFORMATION);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"请先停止当前运行的线程！", L"提示", MB_OK | MB_ICONWARNING);
                    }
                    break;
            }
        }
        break;
        
        case WM_TIMER:  // 定时器消息 - 用于更新界面
            if (g_threadsRunning)
            {
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;

        case WM_DESTROY:  // 窗口销毁消息
        {
            // 停止定时器
            KillTimer(hWnd, 1);
            
            // 停止所有线程
            StopAllThreads();
            
            // 关闭并清除所有线程句柄
            for (size_t i = 0; i < g_threads.size(); i++)
            {
                if (g_threads[i].hThread)
                {
                    CloseHandle(g_threads[i].hThread);
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

// 线程函数
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    ThreadInfo* pThreadInfo = (ThreadInfo*)lpParameter;
    
    while (TRUE)
    {
        // 等待开始事件
        WaitForSingleObject(g_eventStart, INFINITE);
        
        // 检查是否应该停止
        if (WaitForSingleObject(g_eventStop, 0) == WAIT_OBJECT_0)
        {
            pThreadInfo->isRunning = FALSE;
            continue;
        }
        
        pThreadInfo->isRunning = TRUE;
        
        // 模拟工作
        Sleep(100);  // 线程休眠100毫秒
        
        // 使用临界区保护共享资源
        EnterCriticalSection(&g_criticalSection);
        
        // 增加计数器
        pThreadInfo->counter++;
        g_totalCounter++;
        
        // 离开临界区
        LeaveCriticalSection(&g_criticalSection);
    }
    
    return 0;
}

// 创建线程
void CreateThreads(HWND hWnd, int count)
{
    // 停止定时器
    KillTimer(hWnd, 1);
    
    // 创建指定数量的线程
    for (int i = 0; i < count; i++)
    {
        ThreadInfo threadInfo;
        threadInfo.threadNum = ++g_threadCount;
        threadInfo.counter = 0;
        threadInfo.isRunning = FALSE;
        
        // 创建线程
        threadInfo.hThread = CreateThread(
            NULL,                   // 安全属性
            0,                      // 栈大小（默认）
            ThreadProc,             // 线程函数
            &threadInfo,            // 线程参数
            0,                      // 创建标志（立即运行）
            &threadInfo.dwThreadId  // 线程ID
        );
        
        if (threadInfo.hThread)
        {
            g_threads.push_back(threadInfo);
        }
    }
    
    // 启动定时器用于更新界面
    SetTimer(hWnd, 1, 200, NULL);  // 每200毫秒更新一次
}

// 停止所有线程
void StopAllThreads()
{
    g_threadsRunning = FALSE;
    
    // 设置停止事件
    SetEvent(g_eventStop);
    ResetEvent(g_eventStart);
    
    // 更新线程状态
    for (size_t i = 0; i < g_threads.size(); i++)
    {
        g_threads[i].isRunning = FALSE;
    }
}

// 绘制线程信息
void DrawThreadInfo(HDC hdc, RECT* prc)
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
    
    // 绘制线程信息标题
    RECT titleRect = {50, 170, 650, 190};
    DrawTextW(hdc, L"线程状态信息:", -1, &titleRect, DT_LEFT);
    
    // 绘制分隔线
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 50, 195, NULL);
    LineTo(hdc, 650, 195);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    
    // 绘制表头
    WCHAR header[200];
    wsprintfW(header, L"线程ID    线程编号    状态    计数值");
    RECT headerRowRect = {50, 200, 650, 220};
    DrawTextW(hdc, header, -1, &headerRowRect, DT_LEFT);
    
    // 绘制每个线程的信息
    WCHAR info[500] = L"";
    WCHAR threadInfo[200];
    
    for (size_t i = 0; i < g_threads.size(); i++)
    {
        wsprintfW(threadInfo, L"%d      %d          %s    %d\n",
            g_threads[i].dwThreadId,
            g_threads[i].threadNum,
            g_threads[i].isRunning ? L"运行中" : L"已停止",
            g_threads[i].counter
        );
        wcscat_s(info, threadInfo);
    }
    
    // 如果没有线程，显示提示
    if (g_threads.empty())
    {
        wcscpy_s(info, L"当前没有活动的线程。点击'创建线程'按钮创建线程。");
    }
    
    RECT infoRect = {50, 225, 650, 400};
    DrawTextW(hdc, info, -1, &infoRect, DT_LEFT);
    
    // 绘制总计数器
    WCHAR totalInfo[100];
    wsprintfW(totalInfo, L"总计数: %d", g_totalCounter);
    RECT totalRect = {50, 400, 650, 420};
    DrawTextW(hdc, totalInfo, -1, &totalRect, DT_LEFT);
    
    // 清理资源
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// 总结：
// 1. 线程的创建和管理：
//    - 使用CreateThread函数创建线程，指定线程函数和参数
//    - 每个线程有唯一的线程ID和句柄
//    - 使用CloseHandle关闭不再需要的线程句柄
//    - 使用TerminateThread可以强制终止线程（但不推荐）
// 2. 线程同步机制：
//    - 临界区(Critical Section)：用于同一进程内的线程同步
//    - 事件对象(Event)：用于线程间的信号通知
//    - 互斥量(Mutex)：用于跨进程的线程同步
//    - 信号量(Semaphore)：控制对共享资源的访问数量
// 3. 线程同步的重要性：
//    - 防止数据竞争和不一致
//    - 保护共享资源的访问
//    - 确保线程按照预期顺序执行
// 4. 多线程编程的最佳实践：
//    - 最小化共享数据
//    - 保持临界区代码简短
//    - 使用适当的同步机制
//    - 避免死锁和活锁
//    - 正确处理线程的创建和销毁
//    - 考虑线程安全性
// 5. 线程同步注意事项：
//    - 临界区必须配对使用EnterCriticalSection和LeaveCriticalSection
//    - 事件对象可以是手动重置或自动重置的
//    - 避免嵌套锁，防止死锁
//    - 长时间持有锁会影响性能
// 6. 线程池：
//    - 对于大量短期任务，考虑使用线程池
//    - Windows提供了API如QueueUserWorkItem来使用线程池
//    - 线程池可以减少线程创建和销毁的开销
// 7. 多线程的优缺点：
//    - 优点：提高CPU利用率、改善响应性、并行处理任务
//    - 缺点：增加复杂度、同步开销、潜在的并发问题
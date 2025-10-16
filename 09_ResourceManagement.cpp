// 09_ResourceManagement.cpp
// 教学内容：Win32资源管理
// 教学大纲：
// 1. GDI资源的创建和释放
// 2. 资源泄漏的预防
// 3. 资源的共享和重用
// 4. 位图资源的加载和显示
// 5. 资源管理的最佳实践

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
#include <string>     // 包含字符串处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"ResourceManagementWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32资源管理示例";  // 窗口标题（宽字符版本）

// 资源计数器
struct ResourceCounters {
    int gdiObjects;       // GDI对象计数
    int createdPens;      // 创建的画笔数量
    int createdBrushes;   // 创建的画刷数量
    int createdBitmaps;   // 创建的位图数量
    int createdFonts;     // 创建的字体数量
};

// 全局资源计数器
ResourceCounters g_counters = {0, 0, 0, 0, 0};

// 测试资源
HBITMAP g_testBitmap = NULL;  // 测试位图
HPEN g_pens[5] = {NULL};      // 画笔数组
HBRUSH g_brushes[5] = {NULL}; // 画刷数组
HFONT g_fonts[3] = {NULL};    // 字体数组

// 资源状态
BOOL g_resourcesCreated = FALSE;  // 资源是否已创建

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
BOOL CreateTestResources();  // 创建测试资源
VOID ReleaseAllResources();  // 释放所有资源
void DrawResourceInfo(HDC hdc, RECT* prc);  // 绘制资源信息
void DrawTestImage(HDC hdc, RECT* prc);  // 绘制测试图像

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

    // 确保所有资源都被释放
    ReleaseAllResources();

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
                L"BUTTON", L"创建测试资源", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                50, 420, 150, 30, hWnd, (HMENU)1001, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"释放所有资源", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                220, 420, 150, 30, hWnd, (HMENU)1002, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"强制资源泄漏", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                390, 420, 150, 30, hWnd, (HMENU)1003, hInst, NULL
            );
            
            CreateWindowW(
                L"BUTTON", L"刷新显示", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                560, 420, 100, 30, hWnd, (HMENU)1004, hInst, NULL
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
            LPCWSTR instructions = L"资源管理示例演示\n\n操作说明：\n1. 创建测试资源：创建画笔、画刷、位图等GDI资源\n2. 释放所有资源：释放所有已创建的资源\n3. 强制资源泄漏：演示不正确的资源管理导致的泄漏\n4. 刷新显示：更新资源使用状态显示";
            RECT rect = {50, 30, 650, 130};
            DrawTextW(hdc, instructions, -1, &rect, DT_LEFT | DT_WORDBREAK);

            // 绘制资源信息
            DrawResourceInfo(hdc, &rcClient);

            // 如果资源已创建，绘制测试图像
            if (g_resourcesCreated)
            {
                DrawTestImage(hdc, &rcClient);
            }

            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;

        case WM_COMMAND:  // 命令消息
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
                case 1001:  // 创建测试资源
                    if (!g_resourcesCreated)
                    {
                        if (CreateTestResources())
                        {
                            g_resourcesCreated = TRUE;
                            MessageBoxW(hWnd, L"测试资源创建成功！", L"成功", MB_OK | MB_ICONINFORMATION);
                        }
                        else
                        {
                            MessageBoxW(hWnd, L"测试资源创建失败！", L"错误", MB_OK | MB_ICONERROR);
                        }
                    }
                    else
                    {
                        MessageBoxW(hWnd, L"资源已经创建，请先释放！", L"提示", MB_OK | MB_ICONWARNING);
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case 1002:  // 释放所有资源
                    ReleaseAllResources();
                    g_resourcesCreated = FALSE;
                    MessageBoxW(hWnd, L"所有资源已释放！", L"成功", MB_OK | MB_ICONINFORMATION);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case 1003:  // 强制资源泄漏
                    {   
                        // 创建一些资源但不释放，演示资源泄漏
                        int leakCount = 10;
                        for (int i = 0; i < leakCount; i++)
                        {
                            HPEN hLeakPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                            HBRUSH hLeakBrush = CreateSolidBrush(RGB(255, 255, 0));
                            // 故意不释放这些资源
                            g_counters.createdPens++;
                            g_counters.createdBrushes++;
                        }
                        MessageBoxW(hWnd, L"已创建资源但故意不释放，造成资源泄漏！", L"警告", MB_OK | MB_ICONWARNING);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    break;
                    
                case 1004:  // 刷新显示
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
        }
        break;

        case WM_DESTROY:  // 窗口销毁消息
        {
            // 确保所有资源都被释放
            ReleaseAllResources();
            PostQuitMessage(0);  // 向应用程序发送退出消息
        }
        break;

        default:
            // 处理未定义的消息
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;  // 返回处理结果
}

// 创建测试资源
BOOL CreateTestResources()
{
    // 创建画笔
    COLORREF penColors[] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0), RGB(255, 0, 255)};
    int penStyles[] = {PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT};
    int penWidths[] = {1, 2, 3, 4, 5};
    
    for (int i = 0; i < 5; i++)
    {
        g_pens[i] = CreatePen(penStyles[i], penWidths[i], penColors[i]);
        if (!g_pens[i]) return FALSE;
        g_counters.createdPens++;
    }
    
    // 创建画刷
    COLORREF brushColors[] = {RGB(255, 192, 192), RGB(192, 255, 192), RGB(192, 192, 255), 
                             RGB(255, 255, 192), RGB(255, 192, 255)};
    
    for (int i = 0; i < 5; i++)
    {
        g_brushes[i] = CreateSolidBrush(brushColors[i]);
        if (!g_brushes[i]) return FALSE;
        g_counters.createdBrushes++;
    }
    
    // 创建字体
    LPCWSTR fontNames[] = {L"Arial", L"Courier New", L"Times New Roman"};
    int fontSizes[] = {16, 12, 14};
    
    for (int i = 0; i < 3; i++)
    {
        g_fonts[i] = CreateFontW(
            fontSizes[i], 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontNames[i]
        );
        if (!g_fonts[i]) return FALSE;
        g_counters.createdFonts++;
    }
    
    // 创建内存位图作为测试图像
    HDC hdc = GetDC(NULL);
    g_testBitmap = CreateCompatibleBitmap(hdc, 100, 100);
    ReleaseDC(NULL, hdc);
    
    if (!g_testBitmap)
        return FALSE;
    
    g_counters.createdBitmaps++;
    
    // 在创建的位图上绘制内容
    HDC memDC = CreateCompatibleDC(NULL);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, g_testBitmap);
    
    // 填充背景
    HBRUSH backBrush = CreateSolidBrush(RGB(255, 255, 255));
    RECT bitmapRect = {0, 0, 100, 100};
    FillRect(memDC, &bitmapRect, backBrush);
    DeleteObject(backBrush);
    
    // 绘制一些图形
    HPEN rectPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(memDC, rectPen);
    Rectangle(memDC, 10, 10, 90, 90);
    DeleteObject(rectPen);
    
    // 绘制文本
    SelectObject(memDC, g_fonts[0]);
    SetTextColor(memDC, RGB(255, 0, 0));
    SetBkMode(memDC, TRANSPARENT);
    TextOutW(memDC, 25, 40, L"测试位图", 6);
    
    // 恢复原始位图并清理
    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
    
    return TRUE;
}

// 释放所有资源
VOID ReleaseAllResources()
{
    // 释放画笔
    for (int i = 0; i < 5; i++)
    {
        if (g_pens[i])
        {
            DeleteObject(g_pens[i]);
            g_pens[i] = NULL;
            g_counters.createdPens--;
        }
    }
    
    // 释放画刷
    for (int i = 0; i < 5; i++)
    {
        if (g_brushes[i])
        {
            DeleteObject(g_brushes[i]);
            g_brushes[i] = NULL;
            g_counters.createdBrushes--;
        }
    }
    
    // 释放字体
    for (int i = 0; i < 3; i++)
    {
        if (g_fonts[i])
        {
            DeleteObject(g_fonts[i]);
            g_fonts[i] = NULL;
            g_counters.createdFonts--;
        }
    }
    
    // 释放位图
    if (g_testBitmap)
    {
        DeleteObject(g_testBitmap);
        g_testBitmap = NULL;
        g_counters.createdBitmaps--;
    }
    
    // 更新GDI对象计数
    g_counters.gdiObjects = g_counters.createdPens + g_counters.createdBrushes + 
                           g_counters.createdBitmaps + g_counters.createdFonts;
}

// 绘制资源信息
void DrawResourceInfo(HDC hdc, RECT* prc)
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
    
    // 绘制资源信息标题
    RECT titleRect = {50, 150, 650, 170};
    DrawTextW(hdc, L"当前资源使用情况:", -1, &titleRect, DT_LEFT);
    
    // 绘制分隔线
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 50, 175, NULL);
    LineTo(hdc, 650, 175);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    
    // 计算并显示资源计数
    g_counters.gdiObjects = g_counters.createdPens + g_counters.createdBrushes + 
                           g_counters.createdBitmaps + g_counters.createdFonts;
    
    WCHAR info[500];
    wsprintfW(info, 
        L"总GDI对象数: %d\n" 
        L"已创建画笔: %d\n" 
        L"已创建画刷: %d\n" 
        L"已创建位图: %d\n" 
        L"已创建字体: %d\n" 
        L"资源状态: %s", 
        g_counters.gdiObjects,
        g_counters.createdPens,
        g_counters.createdBrushes,
        g_counters.createdBitmaps,
        g_counters.createdFonts,
        g_resourcesCreated ? L"资源已创建" : L"资源未创建"
    );
    
    RECT infoRect = {50, 180, 650, 300};
    DrawTextW(hdc, info, -1, &infoRect, DT_LEFT | DT_WORDBREAK);
    
    // 清理资源
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// 绘制测试图像
void DrawTestImage(HDC hdc, RECT* prc)
{
    // 绘制测试位图
    if (g_testBitmap)
    {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, g_testBitmap);
        
        // 绘制位图到客户区
        BitBlt(hdc, 450, 180, 100, 100, memDC, 0, 0, SRCCOPY);
        
        // 恢复原始位图并清理
        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }
    
    // 使用创建的资源绘制一些图形
    RECT drawRect = {450, 300, 600, 400};
    
    for (int i = 0; i < 5 && g_pens[i] && g_brushes[i]; i++)
    {
        HPEN oldPen = (HPEN)SelectObject(hdc, g_pens[i]);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, g_brushes[i]);
        
        // 绘制不同的图形
        if (i == 0) Rectangle(hdc, 450, 300, 500, 350);
        else if (i == 1) Ellipse(hdc, 510, 300, 560, 350);
        else if (i == 2) RoundRect(hdc, 570, 300, 620, 350, 10, 10);
        else if (i == 3) {
            POINT points[] = {{450, 370}, {475, 330}, {500, 370}};
            Polygon(hdc, points, 3);
        }
        else {
            SelectObject(hdc, g_fonts[2]);
            TextOutW(hdc, 510, 360, L"资源测试", 5);
        }
        
        SelectObject(hdc, oldPen);
        SelectObject(hdc, oldBrush);
    }
}

// 总结：
// 1. GDI资源管理的重要性：
//    - Windows系统对每个进程可以使用的GDI对象数量有限制（默认10000个）
//    - 资源泄漏会导致系统资源耗尽，最终导致应用程序崩溃
//    - 正确管理资源是编写稳定Win32应用程序的关键
// 2. 常见的GDI资源类型：
//    - 画笔(Pen)：用于绘制线条
//    - 画刷(Brush)：用于填充区域
//    - 位图(Bitmap)：用于存储图像
//    - 字体(Font)：用于文本显示
//    - 设备上下文(DC)：绘图的环境
// 3. 资源管理的最佳实践：
//    - 创建资源后，总是确保在不再需要时释放它们
//    - 使用配对的SelectObject和DeleteObject来管理资源
//    - 在窗口销毁时释放所有已创建的资源
//    - 使用RAII模式（如果使用C++）来管理资源生命周期
//    - 尽量重用资源而不是频繁创建和销毁
// 4. 检测资源泄漏的方法：
//    - 使用GDIView等工具监控进程的GDI对象使用情况
//    - 在代码中添加资源计数和跟踪机制
//    - 通过任务管理器的详细信息视图查看GDI对象计数
// 5. 资源共享和优化：
//    - 对于频繁使用的资源，可以创建一次并重复使用
//    - 利用系统预定义的GDI对象（GetStockObject）
//    - 考虑使用内存DC进行双缓冲绘图，减少闪烁并优化性能
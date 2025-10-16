// 07_MouseHandling.cpp
// 教学内容：Win32鼠标输入处理
// 教学大纲：
// 1. 鼠标消息的类型和处理
// 2. 鼠标位置的获取和转换
// 3. 鼠标按钮事件的处理
// 4. 鼠标滚轮事件的处理
// 5. 拖放操作的实现

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
#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wp) ((short)HIWORD(wp))
#endif

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"MouseHandlingWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32鼠标输入处理示例";  // 窗口标题（宽字符版本）

// 鼠标信息结构体
typedef struct {
    POINT position;      // 鼠标位置
    BOOL leftButton;     // 左键状态
    BOOL rightButton;    // 右键状态
    BOOL middleButton;   // 中键状态
    INT wheelDelta;      // 滚轮增量
    WCHAR lastEvent[50]; // 最后事件
} MouseInfo;

// 全局鼠标信息
MouseInfo g_mouseInfo = {{0, 0}, FALSE, FALSE, FALSE, 0, L""};

// 绘图相关变量
POINT g_dragStart = {-1, -1};  // 拖动起始位置
POINT g_dragEnd = {-1, -1};    // 拖动结束位置
BOOL g_isDragging = FALSE;     // 是否正在拖动

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
void DrawMouseInfo(HDC hdc, RECT* prc);  // 绘制鼠标信息

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

            // 绘制说明文本（增加显示区域高度以确保第5条提示完整显示）
            LPCWSTR instructions = L"鼠标操作演示区域\n\n操作提示：\n1. 移动鼠标：显示当前位置\n2. 左键点击：标记点击位置\n3. 右键点击：显示右键菜单\n4. 滚轮滚动：调整颜色\n5. 拖拽：绘制矩形";
            RECT rect = {50, 30, 550, 180}; // 底部坐标从150增加到180
            DrawTextW(hdc, instructions, -1, &rect, DT_LEFT | DT_WORDBREAK);

            // 绘制鼠标信息
            DrawMouseInfo(hdc, &rcClient);

            // 如果正在拖动或已经完成拖动，绘制矩形
            if (g_isDragging || (g_dragStart.x != -1 && g_dragEnd.x != -1))
            {
                HPEN hPen = CreatePen(PS_DASH, 1, RGB(255, 0, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

                Rectangle(hdc, g_dragStart.x, g_dragStart.y, g_dragEnd.x, g_dragEnd.y);

                SelectObject(hdc, hOldPen);
                SelectObject(hdc, hOldBrush);
                DeleteObject(hPen);
            }

            EndPaint(hWnd, &ps);  // 结束绘图
        }
        break;

        case WM_MOUSEMOVE:  // 鼠标移动消息
        {
            // 从lParam中提取鼠标位置
            g_mouseInfo.position.x = GET_X_LPARAM(lParam);
            g_mouseInfo.position.y = GET_Y_LPARAM(lParam);
            
            // 检查鼠标按钮状态
            g_mouseInfo.leftButton = (wParam & MK_LBUTTON) != 0;
            g_mouseInfo.rightButton = (wParam & MK_RBUTTON) != 0;
            g_mouseInfo.middleButton = (wParam & MK_MBUTTON) != 0;
            
            // 更新最后事件
            wcscpy_s(g_mouseInfo.lastEvent, L"鼠标移动");
            
            // 如果正在拖动，更新拖动结束位置
            if (g_isDragging)
            {
                g_dragEnd.x = g_mouseInfo.position.x;
                g_dragEnd.y = g_mouseInfo.position.y;
                wcscpy_s(g_mouseInfo.lastEvent, L"拖动中");
            }
            
            // 重绘窗口，使用TRUE确保清除背景防止文字重叠
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_LBUTTONDOWN:  // 左键按下消息
        {
            // 记录拖动起始位置
            g_dragStart.x = GET_X_LPARAM(lParam);
            g_dragStart.y = GET_Y_LPARAM(lParam);
            g_dragEnd.x = g_dragStart.x;
            g_dragEnd.y = g_dragStart.y;
            g_isDragging = TRUE;
            
            // 更新鼠标信息
            g_mouseInfo.leftButton = TRUE;
            wcscpy_s(g_mouseInfo.lastEvent, L"左键按下");
            
            // 设置捕获鼠标
            SetCapture(hWnd);
            
            // 重绘窗口，使用TRUE确保清除背景防止文字重叠
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_LBUTTONUP:  // 左键释放消息
        {
            // 停止拖动
            if (g_isDragging)
            {
                g_dragEnd.x = GET_X_LPARAM(lParam);
                g_dragEnd.y = GET_Y_LPARAM(lParam);
                g_isDragging = FALSE;
                wcscpy_s(g_mouseInfo.lastEvent, L"左键释放");
                
                // 释放鼠标捕获
                ReleaseCapture();
                
                // 重绘窗口
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else
            {
                // 只是点击，不是拖动
                wcscpy_s(g_mouseInfo.lastEvent, L"左键点击");
                InvalidateRect(hWnd, NULL, TRUE);
            }
            
            g_mouseInfo.leftButton = FALSE;
        }
        break;

        case WM_RBUTTONDOWN:  // 右键按下消息
        {
            g_mouseInfo.rightButton = TRUE;
            wcscpy_s(g_mouseInfo.lastEvent, L"右键按下");
            
            // 创建右键菜单
            HMENU hPopupMenu = CreatePopupMenu();
            AppendMenuW(hPopupMenu, MF_STRING, 1, L"清空绘图");
            AppendMenuW(hPopupMenu, MF_STRING, 2, L"获取帮助");
            AppendMenuW(hPopupMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuW(hPopupMenu, MF_STRING, 3, L"退出");
            
            // 获取当前鼠标位置
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            
            // 转换为屏幕坐标
            ClientToScreen(hWnd, &pt);
            
            // 显示右键菜单
            TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
            
            // 销毁菜单
            DestroyMenu(hPopupMenu);
            
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_RBUTTONUP:  // 右键释放消息
        {
            g_mouseInfo.rightButton = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_MBUTTONDOWN:  // 中键按下消息
        {
            g_mouseInfo.middleButton = TRUE;
            wcscpy_s(g_mouseInfo.lastEvent, L"中键按下");
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_MBUTTONUP:  // 中键释放消息
        {
            g_mouseInfo.middleButton = FALSE;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_MOUSEWHEEL:  // 鼠标滚轮消息
        {
            // 获取滚轮增量
            g_mouseInfo.wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            
            // 更新最后事件
            if (g_mouseInfo.wheelDelta > 0)
                wcscpy_s(g_mouseInfo.lastEvent, L"滚轮向前");
            else
                wcscpy_s(g_mouseInfo.lastEvent, L"滚轮向后");
            
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

        case WM_COMMAND:  // 命令消息（处理右键菜单命令）
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
                case 1:  // 清空绘图
                    g_dragStart.x = g_dragStart.y = -1;
                    g_dragEnd.x = g_dragEnd.y = -1;
                    g_isDragging = FALSE;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case 2:  // 获取帮助
                    MessageBoxW(hWnd, L"鼠标操作演示帮助\n\n功能说明：\n1. 移动鼠标可在状态区域显示当前位置\n2. 左键点击标记点击位置\n3. 左键拖拽可绘制矩形\n4. 右键点击显示上下文菜单\n5. 鼠标滚轮滚动可记录滚轮增量", 
                        L"帮助", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case 3:  // 退出
                    DestroyWindow(hWnd);
                    break;
            }
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

// 绘制鼠标信息
void DrawMouseInfo(HDC hdc, RECT* prc)
{
    // 创建字体（减小字体大小以显示更多内容）
    HFONT hFont = CreateFontW(
        12,  // 字体高度（从14减小到12）
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
    
    // 定义信息文本，使用左对齐和固定宽度格式确保数字变化时不会重叠
    WCHAR info[500];
    wsprintfW(info, 
        L"鼠标状态信息：\n" 
        L"- 当前位置：X=%-6d, Y=%-6d\n" 
        L"- 左键状态：%s\n" 
        L"- 右键状态：%s\n" 
        L"- 中键状态：%s\n" 
        L"- 滚轮增量：%-6d\n" 
        L"- 最后事件：%s\n", 
        g_mouseInfo.position.x, g_mouseInfo.position.y,
        g_mouseInfo.leftButton ? L"按下" : L"释放",
        g_mouseInfo.rightButton ? L"按下" : L"释放",
        g_mouseInfo.middleButton ? L"按下" : L"释放",
        g_mouseInfo.wheelDelta,
        g_mouseInfo.lastEvent
    );
    
    // 绘制信息文本（扩大显示区域以确保所有文字完整显示）
    RECT infoRect = {50, 180, 550, 450};
    DrawTextW(hdc, info, -1, &infoRect, DT_LEFT | DT_WORDBREAK);
    
    // 清理资源
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}

// 总结：
// 1. 鼠标输入处理主要通过以下消息：
//    - WM_MOUSEMOVE：鼠标移动
//    - WM_LBUTTONDOWN/WM_LBUTTONUP：左键按下/释放
//    - WM_RBUTTONDOWN/WM_RBUTTONUP：右键按下/释放
//    - WM_MBUTTONDOWN/WM_MBUTTONUP：中键按下/释放
//    - WM_MOUSEWHEEL：鼠标滚轮滚动
// 2. 鼠标位置可以从消息的lParam参数中提取，使用GET_X_LPARAM和GET_Y_LPARAM宏
// 3. 鼠标按钮状态可以从wParam参数中检查，使用MK_LBUTTON、MK_RBUTTON、MK_MBUTTON等
// 4. 鼠标滚轮增量可以通过GET_WHEEL_DELTA_WPARAM宏从wParam中提取
// 5. 拖放操作通常通过WM_LBUTTONDOWN、WM_MOUSEMOVE和WM_LBUTTONUP消息组合实现
// 6. 右键菜单可以通过WM_RBUTTONDOWN消息和TrackPopupMenu函数实现
// 7. 当进行拖拽操作时，建议使用SetCapture和ReleaseCapture来捕获鼠标
// 8. 鼠标坐标在不同消息中可能是客户区坐标或屏幕坐标，需要根据情况进行转换
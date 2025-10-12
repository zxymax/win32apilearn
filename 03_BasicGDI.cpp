// 03_BasicGDI.cpp
// 教学内容：Win32 GDI（图形设备接口）基础绘图
// 教学大纲：
// 1. 设备上下文(DC)的获取和使用
// 2. 画笔和画刷的创建与使用
// 3. 基本图形的绘制（线条、矩形、椭圆等）
// 4. 文本的绘制和格式化
// 5. 绘图区域的设置

// 定义使用Unicode字符集（必须在包含windows.h之前定义）
#define UNICODE
#define _UNICODE

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"BasicGDIWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32 GDI基础绘图示例";  // 窗口标题（宽字符版本）

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
void DrawShapes(HDC hdc, RECT* prc);  // 绘图函数声明

// WinMain函数 - Win32应用程序的入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 窗口类结构体定义
    WNDCLASSEX wcex;

    // 填充窗口类结构体的各个成员
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // 注册窗口类
        if (!RegisterClassEx(&wcex))
        {
            MessageBoxW(NULL, L"窗口类注册失败！", L"错误", MB_ICONERROR);
            return 1;
        }

    // 保存应用程序实例句柄
    hInst = hInstance;

    // 创建窗口（宽字符版本）
    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
        NULL, NULL, hInstance, NULL
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
        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;  // 绘图结构体
            HDC hdc = BeginPaint(hWnd, &ps);  // 开始绘图，获取设备上下文

            // 获取客户区矩形
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);

            // 调用绘图函数绘制各种图形
            DrawShapes(hdc, &rcClient);

            EndPaint(hWnd, &ps);  // 结束绘图
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

// 绘图函数 - 绘制各种基本图形
// 参数说明：
// hdc: 设备上下文句柄，用于绘图操作
// prc: 客户区矩形指针，用于确定绘图区域
void DrawShapes(HDC hdc, RECT* prc)
{
    // 1. 创建画笔和画刷
    // 创建红色实线画笔（宽度为2）
    HPEN hPenRed = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    // 创建蓝色实线画笔（宽度为1）
    HPEN hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    // 创建绿色画刷（用于填充）
    HBRUSH hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
    // 创建黄色画刷（用于填充）
    HBRUSH hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));
    // 创建透明画刷（用于不填充）
    HBRUSH hBrushNull = (HBRUSH)GetStockObject(NULL_BRUSH);

    // 2. 绘制线条
    // 选择红色画笔到设备上下文
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPenRed);
    // 绘制一条直线（从点(50,50)到点(200,50)）
    MoveToEx(hdc, 50, 50, NULL);
    LineTo(hdc, 200, 50);

    // 3. 绘制矩形
    // 选择蓝色画笔到设备上下文
    SelectObject(hdc, hPenBlue);
    // 选择绿色画刷到设备上下文
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushGreen);
    // 绘制一个填充的矩形（左上角(50,80)，右下角(200,150)）
    Rectangle(hdc, 50, 80, 200, 150);

    // 4. 绘制椭圆
    // 选择黄色画刷到设备上下文
    SelectObject(hdc, hBrushYellow);
    // 绘制一个填充的椭圆（外切矩形左上角(250,80)，右下角(400,150)）
    Ellipse(hdc, 250, 80, 400, 150);

    // 5. 绘制圆角矩形
    // 选择透明画刷到设备上下文
    SelectObject(hdc, hBrushNull);
    // 绘制一个带圆角的矩形（左上角(50,200)，右下角(200,270)，圆角宽高各为20）
    RoundRect(hdc, 50, 200, 200, 270, 20, 20);

    // 6. 绘制多边形
    // 定义多边形的顶点数组
    POINT points[4] = {{250, 200}, {325, 150}, {400, 200}, {325, 270}};
    // 绘制一个多边形
    Polygon(hdc, points, 4);

    // 7. 绘制文本
    // 设置文本颜色为蓝色
    SetTextColor(hdc, RGB(0, 0, 128));
    // 设置文本背景模式为透明
    SetBkMode(hdc, TRANSPARENT);
    // 创建字体（使用宽字符版本的函数）
    HFONT hFont = CreateFontW(
        20,  // 字体高度
        0,   // 字体宽度（0表示自动调整）
        0,   // 文本倾斜角度
        0,   // 文本方向角度
        FW_BOLD,  // 字体粗细
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
    // 选择字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    // 定义要绘制的文本（宽字符版本）
    LPCWSTR text = L"Win32 GDI基础绘图示例";
    // 绘制文本（使用宽字符版本的函数）
    TextOutW(hdc, 150, 320, text, wcslen(text));

    // 8. 清理资源 - 恢复原来的GDI对象并删除创建的对象
    SelectObject(hdc, hOldPen);  // 恢复原来的画笔
    SelectObject(hdc, hOldBrush);  // 恢复原来的画刷
    SelectObject(hdc, hOldFont);  // 恢复原来的字体
    DeleteObject(hPenRed);  // 删除红色画笔
    DeleteObject(hPenBlue);  // 删除蓝色画笔
    DeleteObject(hBrushGreen);  // 删除绿色画刷
    DeleteObject(hBrushYellow);  // 删除黄色画刷
    DeleteObject(hFont);  // 删除字体
    // 注意：不需要删除hBrushNull，因为它是系统预定义的对象
}

// 总结：
// 1. 设备上下文(DC)：GDI绘图的核心对象，代表绘图表面，提供绘图函数所需的环境
// 2. GDI对象：
//    - 画笔(Pen)：用于绘制线条和形状的轮廓
//    - 画刷(Brush)：用于填充形状内部
//    - 字体(Font)：用于文本绘制
// 3. 常用绘图函数：
//    - MoveToEx/LineTo：绘制线条
//    - Rectangle：绘制矩形
//    - Ellipse：绘制椭圆
//    - RoundRect：绘制圆角矩形
//    - Polygon：绘制多边形
//    - TextOut：绘制文本
// 4. 绘图属性设置：
//    - SetTextColor：设置文本颜色
//    - SetBkMode：设置文本背景模式
// 5. GDI资源管理：
//    - 创建的GDI对象使用完后必须删除，以避免资源泄漏
//    - 使用SelectObject函数保存和恢复原来的GDI对象
// 6. 系统预定义GDI对象：
//    - 可以通过GetStockObject函数获取，如NULL_BRUSH（透明画刷）
// 7. 注意事项：
//    - 绘图操作通常在WM_PAINT消息处理中进行
//    - 必须使用BeginPaint和EndPaint函数来获取和释放设备上下文
//    - 绘制前应保存原来的GDI对象，并在绘制后恢复
//    - 所有创建的GDI对象都必须调用DeleteObject函数删除
//    - 坐标系统以客户区左上角为原点，X轴向右，Y轴向下
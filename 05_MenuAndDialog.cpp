// 05_MenuAndDialog.cpp
// 教学内容：Win32菜单和对话框的创建与使用
// 教学大纲：
// 1. 菜单资源的定义和加载
// 2. 窗口菜单的创建和设置
// 3. 弹出菜单的创建和显示
// 4. 模态对话框的创建和使用
// 5. 非模态对话框的创建和使用
// 6. 菜单命令和对话框消息的处理

// 定义使用Unicode字符集（必须在包含windows.h之前定义）
#define UNICODE
#define _UNICODE

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"MenuAndDialogWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32菜单和对话框示例";  // 窗口标题（宽字符版本）

// 对话框ID定义
#define IDD_ABOUT 101  // 关于对话框ID
#define IDD_SETTINGS 102  // 设置对话框ID

// 菜单ID定义
#define ID_FILE_EXIT 40001  // 文件-退出菜单项ID
#define ID_HELP_ABOUT 40002  // 帮助-关于菜单项ID
#define ID_EDIT_COPY 40003  // 编辑-复制菜单项ID
#define ID_EDIT_PASTE 40004  // 编辑-粘贴菜单项ID
#define ID_VIEW_SETTINGS 40005  // 视图-设置菜单项ID

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);  // 关于对话框过程函数声明
INT_PTR CALLBACK SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);  // 设置对话框过程函数声明
HMENU CreateMainMenu();  // 创建主菜单函数声明

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
    wcex.lpszMenuName = NULL;  // 这里不使用资源文件中的菜单，而是动态创建
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // 注册窗口类
    if (!RegisterClassEx(&wcex))
    {
        // 注册失败时显示错误消息（使用宽字符版本）
        MessageBoxW(NULL, L"窗口类注册失败！", L"错误", MB_ICONERROR);
        return 1;
    }

    // 保存应用程序实例句柄
    hInst = hInstance;

    // 创建主菜单
    HMENU hMenu = CreateMainMenu();

    // 创建窗口（使用宽字符版本）
    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, hMenu, hInstance, NULL
    );

    // 检查窗口是否创建成功
    if (!hWnd)
    {
        // 创建失败时显示错误消息（使用宽字符版本）
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

    // 销毁菜单
    DestroyMenu(hMenu);

    return (int)msg.wParam;
}

// 创建主菜单函数
// 返回值：创建的主菜单句柄
HMENU CreateMainMenu()
{
    // 创建主菜单
    HMENU hMenu = CreateMenu();
    
    // 创建文件菜单
    HMENU hFileMenu = CreatePopupMenu();
    AppendMenuW(hFileMenu, MF_STRING, ID_FILE_EXIT, L"退出(&X)");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"文件(&F)");
    
    // 创建编辑菜单
    HMENU hEditMenu = CreatePopupMenu();
    AppendMenuW(hEditMenu, MF_STRING, ID_EDIT_COPY, L"复制(&C)");
    AppendMenuW(hEditMenu, MF_STRING, ID_EDIT_PASTE, L"粘贴(&V)");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"编辑(&E)");
    
    // 创建视图菜单
    HMENU hViewMenu = CreatePopupMenu();
    AppendMenuW(hViewMenu, MF_STRING, ID_VIEW_SETTINGS, L"设置(&S)");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hViewMenu, L"视图(&V)");
    
    // 创建帮助菜单
    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenuW(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"关于(&A)...");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"帮助(&H)");
    
    return hMenu;
}

// 窗口过程函数 - 处理发送到窗口的所有消息
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 静态变量，用于存储非模态对话框的句柄
    static HWND hSettingsDlg = NULL;

    // 根据消息类型执行不同的处理
    switch (message)
    {
        case WM_COMMAND:  // 命令消息，通常由菜单或控件发送
        {
            // 获取命令ID
            int wmId = LOWORD(wParam);
            // 获取通知代码
            int wmEvent = HIWORD(wParam);

            // 根据命令ID处理命令
            switch (wmId)
            {
                case ID_FILE_EXIT:  // 文件-退出命令
                    // 销毁窗口，触发WM_DESTROY消息
                    DestroyWindow(hWnd);
                    break;

                case ID_HELP_ABOUT:  // 帮助-关于命令
                    // 使用MessageBoxW显示关于信息（替代不存在的对话框资源）
                        MessageBoxW(hWnd, L"Win32菜单和对话框示例\n版本: 1.0\n\n这是一个演示Win32菜单和对话框功能的示例程序。", L"关于", MB_OK | MB_ICONINFORMATION);
                        break;

                case ID_VIEW_SETTINGS:  // 视图-设置命令
                    // 使用MessageBoxW显示设置信息（替代不存在的对话框资源）
                        MessageBoxW(hWnd, L"设置选项\n\n这里是程序的设置选项。\n在实际应用中，这将是一个包含各种设置控件的对话框。", L"设置", MB_OK | MB_ICONINFORMATION);
                        break;

                case ID_EDIT_COPY:  // 编辑-复制命令
                    // 显示提示消息（宽字符版本）
                        MessageBoxW(hWnd, L"复制命令被执行！", L"信息", MB_OK | MB_ICONINFORMATION);
                        break;

                case ID_EDIT_PASTE:  // 编辑-粘贴命令
                    // 显示提示消息（宽字符版本）
                        MessageBoxW(hWnd, L"粘贴命令被执行！", L"信息", MB_OK | MB_ICONINFORMATION);
                        break;

                default:
                    // 处理未定义的命令
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

        case WM_CONTEXTMENU:  // 右键菜单消息
        {
            // 获取鼠标位置
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            
            // 转换为屏幕坐标
            ClientToScreen(hWnd, &pt);
            
            // 创建右键菜单
            HMENU hPopupMenu = CreatePopupMenu();
            AppendMenuW(hPopupMenu, MF_STRING, ID_EDIT_COPY, L"复制(&C)");
            AppendMenuW(hPopupMenu, MF_STRING, ID_EDIT_PASTE, L"粘贴(&V)");
            AppendMenuW(hPopupMenu, MF_SEPARATOR, 0, NULL);  // 添加分隔线
            AppendMenuW(hPopupMenu, MF_STRING, ID_VIEW_SETTINGS, L"设置(&S)");
            AppendMenuW(hPopupMenu, MF_STRING, ID_HELP_ABOUT, L"关于(&A)...");
            
            // 显示右键菜单
            TrackPopupMenu(
                hPopupMenu,  // 右键菜单句柄
                TPM_RIGHTBUTTON,  // 右键弹出
                pt.x, pt.y,  // 弹出位置
                0,  // 保留参数
                hWnd,  // 父窗口句柄
                NULL  // 菜单区域矩形（无）
            );
            
            // 销毁右键菜单
            DestroyMenu(hPopupMenu);
        }
        break;

        case WM_PAINT:  // 窗口重绘消息
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // 绘制说明文本
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            
            // 设置文本颜色
            SetTextColor(hdc, RGB(0, 0, 128));
            // 设置文本背景模式为透明
            SetBkMode(hdc, TRANSPARENT);
            
            // 绘制标题文本（使用宽字符）
            LPCWSTR titleText = L"Win32菜单和对话框示例程序";
            TextOutW(hdc, 150, 50, titleText, wcslen(titleText));
            
            // 绘制操作说明（使用宽字符）
            LPCWSTR helpText1 = L"操作说明：";
            LPCWSTR helpText2 = L"1. 使用菜单栏访问各种功能";
            LPCWSTR helpText3 = L"2. 在窗口中右键点击显示快捷菜单";
            LPCWSTR helpText4 = L"3. 点击帮助->关于查看程序信息";
            LPCWSTR helpText5 = L"4. 点击视图->设置打开设置对话框";
            
            TextOutW(hdc, 50, 100, helpText1, wcslen(helpText1));
            TextOutW(hdc, 50, 130, helpText2, wcslen(helpText2));
            TextOutW(hdc, 50, 150, helpText3, wcslen(helpText3));
            TextOutW(hdc, 50, 170, helpText4, wcslen(helpText4));
            TextOutW(hdc, 50, 190, helpText5, wcslen(helpText5));

            EndPaint(hWnd, &ps);
        }
        break;

        case WM_DESTROY:  // 窗口销毁消息
            // 发送退出消息
            PostQuitMessage(0);
            break;

        default:
            // 处理未定义的消息
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 注意：由于没有资源文件(.rc)，我们使用MessageBoxW替代了对话框功能
// 在实际应用中，应该创建资源文件来定义对话框，而不是完全通过代码创建

// 总结：
// 1. 菜单创建和使用：
//    - CreateMenu：创建主菜单
//    - CreatePopupMenu：创建弹出菜单
//    - AppendMenuW：向菜单添加菜单项（宽字符版本）
//    - TrackPopupMenu：显示右键菜单
//    - DestroyMenu：销毁菜单
// 2. 对话框类型：
//    - 模态对话框：使用DialogBoxW函数创建（宽字符版本），会阻塞主窗口的消息处理
//    - 非模态对话框：使用CreateDialogW函数创建（宽字符版本），可以与主窗口并行操作
// 3. 对话框过程函数：
//    - 类似于窗口过程函数，但处理对话框特定的消息
//    - WM_INITDIALOG：对话框初始化消息
//    - IDOK和IDCANCEL：确定和取消按钮的命令ID
//    - EndDialog：关闭模态对话框
//    - DestroyWindow：关闭非模态对话框
// 4. 菜单命令处理：
//    - WM_COMMAND消息：处理菜单命令和控件通知
//    - LOWORD(wParam)：获取命令ID
// 5. 右键菜单：
//    - WM_CONTEXTMENU消息：处理右键点击事件
//    - ClientToScreen：将客户端坐标转换为屏幕坐标
// 6. Unicode支持：
//    - 通过定义UNICODE和_UNICODE宏启用Unicode支持
//    - 使用宽字符函数（如CreateWindowW、MessageBoxW、TextOutW等）
//    - 使用宽字符字符串（L前缀）替代TEXT宏
//    - 使用wcslen函数处理宽字符串长度
// 7. 注意事项：
//    - 菜单和对话框使用完后必须销毁，避免资源泄漏
//    - 非模态对话框需要特殊处理，确保不会创建多个实例
//    - 菜单项可以使用(&字母)来设置加速键
//    - 实际应用中，对话框通常通过资源文件(.rc)定义，这里为了简化直接在代码中处理
//    - 在实际开发中，应该创建资源文件来定义菜单和对话框，而不是完全通过代码创建
//    - 使用Unicode编码支持可以确保中文等非ASCII字符正确显示和处理
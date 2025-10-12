// 04_FileOperations.cpp
// 教学内容：Win32文件操作基础
// 教学大纲：
// 1. 文件的创建和打开
// 2. 文件的读取和写入
// 3. 文件指针的定位
// 4. 文件信息的获取
// 5. 文件的关闭

// 定义Unicode支持宏（必须在包含windows.h前定义）
#define UNICODE
#define _UNICODE

#include <windows.h>  // 包含Win32 API头文件
#include <wchar.h>    // 包含宽字符处理函数
#include <tchar.h>    // 包含TCHAR相关的宏定义

// 全局变量声明
HINSTANCE hInst;  // 应用程序实例句柄
LPCWSTR szWindowClass = L"FileOperationsWindow";  // 窗口类名（宽字符版本）
LPCWSTR szTitle = L"Win32文件操作示例";  // 窗口标题（宽字符版本）

// 控件ID定义
#define ID_BUTTON_CREATE 1001  // 创建文件按钮ID
#define ID_BUTTON_WRITE  1002  // 写入文件按钮ID
#define ID_BUTTON_READ   1003  // 读取文件按钮ID
#define ID_BUTTON_INFO   1004  // 获取文件信息按钮ID
#define ID_EDIT_CONTENT  1005  // 内容编辑框ID
#define ID_STATIC_STATUS 1006  // 状态显示静态文本ID

// 函数声明
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 窗口过程函数声明
BOOL CreateSampleFile(LPCWSTR lpFileName);  // 创建文件函数声明（宽字符版本）
BOOL WriteToFile(LPCWSTR lpFileName, LPCWSTR lpContent);  // 写入文件函数声明（宽字符版本）
BOOL ReadFromFile(LPCWSTR lpFileName, LPWSTR lpBuffer, DWORD dwBufferSize);  // 读取文件函数声明（宽字符版本）
BOOL GetFileInformation(LPCWSTR lpFileName, LPWSTR lpInfoBuffer, DWORD dwBufferSize);  // 获取文件信息函数声明（宽字符版本）

// WinMain函数 - Win32应用程序的入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 窗口类结构体定义（宽字符版本）
    WNDCLASSEXW wcex;

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
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
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
        case WM_CREATE:  // 窗口创建时发送的消息
        {
            // 创建状态显示静态文本（宽字符版本）
            CreateWindowW(
                L"STATIC", 
                L"就绪：请点击按钮进行文件操作。", 
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                20, 20, 500, 25, 
                hWnd, 
                (HMENU)ID_STATIC_STATUS, 
                hInst, 
                NULL
            );

            // 创建内容编辑框（宽字符版本）
            CreateWindowW(
                L"EDIT", 
                L"这是要写入文件的示例文本。\r\n您可以修改此内容后点击写入按钮。", 
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
                20, 50, 540, 150, 
                hWnd, 
                (HMENU)ID_EDIT_CONTENT, 
                hInst, 
                NULL
            );

            // 创建文件操作按钮（宽字符版本）
            CreateWindowW(
                L"BUTTON", 
                L"1. 创建文件", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                20, 220, 120, 30, 
                hWnd, 
                (HMENU)ID_BUTTON_CREATE, 
                hInst, 
                NULL
            );

            CreateWindowW(
                L"BUTTON", 
                L"2. 写入文件", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                160, 220, 120, 30, 
                hWnd, 
                (HMENU)ID_BUTTON_WRITE, 
                hInst, 
                NULL
            );

            CreateWindowW(
                L"BUTTON", 
                L"3. 读取文件", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                300, 220, 120, 30, 
                hWnd, 
                (HMENU)ID_BUTTON_READ, 
                hInst, 
                NULL
            );

            CreateWindowW(
                L"BUTTON", 
                L"4. 文件信息", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                440, 220, 120, 30, 
                hWnd, 
                (HMENU)ID_BUTTON_INFO, 
                hInst, 
                NULL
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
                case ID_BUTTON_CREATE:  // 创建文件按钮
                    if (wmEvent == BN_CLICKED)
                    {
                        // 定义文件名（宽字符版本）
                        LPCWSTR lpFileName = L"sample.txt";
                        // 创建文件
                        if (CreateSampleFile(lpFileName))
                        {
                            // 更新状态文本（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"成功：文件已创建！");
                        }
                        else
                        {
                            // 显示错误消息（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"失败：文件创建失败！");
                            MessageBoxW(hWnd, L"无法创建文件！", L"错误", MB_ICONERROR);
                        }
                    }
                    break;

                case ID_BUTTON_WRITE:  // 写入文件按钮
                    if (wmEvent == BN_CLICKED)
                    {
                        // 定义文件名（宽字符版本）
                        LPCWSTR lpFileName = L"sample.txt";
                        // 获取编辑框中的内容（宽字符版本）
                        WCHAR szContent[1024];
                        GetDlgItemTextW(hWnd, ID_EDIT_CONTENT, szContent, ARRAYSIZE(szContent));
                        // 写入文件
                        if (WriteToFile(lpFileName, szContent))
                        {
                            // 更新状态文本（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"成功：内容已写入文件！");
                        }
                        else
                        {
                            // 显示错误消息（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"失败：内容写入失败！");
                            MessageBoxW(hWnd, L"无法写入文件！", L"错误", MB_ICONERROR);
                        }
                    }
                    break;

                case ID_BUTTON_READ:  // 读取文件按钮
                    if (wmEvent == BN_CLICKED)
                    {
                        // 定义文件名（宽字符版本）
                        LPCWSTR lpFileName = L"sample.txt";
                        // 定义缓冲区用于存储读取的内容（宽字符版本）
                        WCHAR szBuffer[1024];
                        // 读取文件
                        if (ReadFromFile(lpFileName, szBuffer, sizeof(szBuffer) / sizeof(TCHAR)))
                        {
                            // 更新编辑框内容（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_EDIT_CONTENT, szBuffer);
                            // 更新状态文本（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"成功：文件内容已读取！");
                        }
                        else
                        {
                            // 显示错误消息（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"失败：文件读取失败！");
                            MessageBoxW(hWnd, L"无法读取文件！", L"错误", MB_ICONERROR);
                        }
                    }
                    break;

                case ID_BUTTON_INFO:  // 获取文件信息按钮
                    if (wmEvent == BN_CLICKED)
                    {
                        // 定义文件名（宽字符版本）
                        LPCWSTR lpFileName = L"sample.txt";
                        // 定义缓冲区用于存储文件信息（宽字符版本）
                        WCHAR szInfoBuffer[1024];
                        // 获取文件信息
                        if (GetFileInformation(lpFileName, szInfoBuffer, sizeof(szInfoBuffer) / sizeof(TCHAR)))
                        {
                            // 显示文件信息（宽字符版本）
                            MessageBoxW(hWnd, szInfoBuffer, L"文件信息", MB_OK | MB_ICONINFORMATION);
                            // 更新状态文本（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"成功：文件信息已获取！");
                        }
                        else
                        {
                            // 显示错误消息（宽字符版本）
                            SetDlgItemTextW(hWnd, ID_STATIC_STATUS, L"失败：获取文件信息失败！");
                            MessageBoxW(hWnd, L"无法获取文件信息！", L"错误", MB_ICONERROR);
                        }
                    }
                    break;

                default:
                    // 处理未定义的命令
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
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

// 创建文件函数
// 参数说明：
// lpFileName: 要创建的文件名（宽字符版本）
// 返回值：成功返回TRUE，失败返回FALSE
BOOL CreateSampleFile(LPCWSTR lpFileName)
{
    // 创建或打开文件（宽字符版本）
    HANDLE hFile = CreateFileW(
        lpFileName,  // 文件名
        GENERIC_WRITE,  // 访问权限：写入
        0,  // 共享模式：不共享
        NULL,  // 安全属性：默认
        CREATE_ALWAYS,  // 创建方式：总是创建（如果已存在则覆盖）
        FILE_ATTRIBUTE_NORMAL,  // 文件属性：普通
        NULL  // 模板文件：无
    );

    // 检查文件是否创建成功
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;  // 创建失败
    }

    // 关闭文件句柄
    CloseHandle(hFile);
    return TRUE;  // 创建成功
}

// 写入文件函数
// 参数说明：
// lpFileName: 要写入的文件名（宽字符版本）
// lpContent: 要写入的内容（宽字符版本）
// 返回值：成功返回TRUE，失败返回FALSE
BOOL WriteToFile(LPCWSTR lpFileName, LPCWSTR lpContent)
{
    // 打开文件用于写入（宽字符版本）
    HANDLE hFile = CreateFileW(
        lpFileName,  // 文件名
        GENERIC_WRITE,  // 访问权限：写入
        0,  // 共享模式：不共享
        NULL,  // 安全属性：默认
        CREATE_ALWAYS,  // 创建方式：总是创建（如果已存在则覆盖）
        FILE_ATTRIBUTE_NORMAL,  // 文件属性：普通
        NULL  // 模板文件：无
    );

    // 检查文件是否打开成功
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;  // 打开失败
    }

    // 写入文件内容
    DWORD dwBytesWritten;
    BOOL bResult = WriteFile(
        hFile,  // 文件句柄
        lpContent,  // 要写入的数据
        wcslen(lpContent) * sizeof(WCHAR),  // 数据大小（字节数，使用宽字符长度计算）
        &dwBytesWritten,  // 实际写入的字节数
        NULL  // 重叠结构：无
    );

    // 关闭文件句柄
    CloseHandle(hFile);
    return bResult;  // 返回写入结果
}

// 读取文件函数
// 参数说明：
// lpFileName: 要读取的文件名（宽字符版本）
// lpBuffer: 用于存储读取内容的缓冲区（宽字符版本）
// dwBufferSize: 缓冲区大小（字符数）
// 返回值：成功返回TRUE，失败返回FALSE
BOOL ReadFromFile(LPCWSTR lpFileName, LPWSTR lpBuffer, DWORD dwBufferSize)
{
    // 打开文件用于读取（宽字符版本）
    HANDLE hFile = CreateFileW(
        lpFileName,  // 文件名
        GENERIC_READ,  // 访问权限：读取
        0,  // 共享模式：不共享
        NULL,  // 安全属性：默认
        OPEN_EXISTING,  // 创建方式：仅打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,  // 文件属性：普通
        NULL  // 模板文件：无
    );

    // 检查文件是否打开成功
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;  // 打开失败
    }

    // 读取文件内容
    DWORD dwBytesRead;
    BOOL bResult = ReadFile(
        hFile,  // 文件句柄
        lpBuffer,  // 存储数据的缓冲区
        (dwBufferSize - 1) * sizeof(WCHAR),  // 要读取的最大字节数（留一个位置给结束符）
        &dwBytesRead,  // 实际读取的字节数
        NULL  // 重叠结构：无
    );

    // 如果读取成功，添加字符串结束符
    if (bResult)
    {
        // 计算实际读取的字符数
        DWORD dwCharsRead = dwBytesRead / sizeof(WCHAR);
        // 添加字符串结束符
        lpBuffer[dwCharsRead] = L'\0';
    }

    // 关闭文件句柄
    CloseHandle(hFile);
    return bResult;  // 返回读取结果
}

// 获取文件信息函数
// 参数说明：
// lpFileName: 要获取信息的文件名（宽字符版本）
// lpInfoBuffer: 用于存储文件信息的缓冲区（宽字符版本）
// dwBufferSize: 缓冲区大小（字符数）
// 返回值：成功返回TRUE，失败返回FALSE
BOOL GetFileInformation(LPCWSTR lpFileName, LPWSTR lpInfoBuffer, DWORD dwBufferSize)
{
    // 打开文件获取句柄（宽字符版本）
    HANDLE hFile = CreateFileW(
        lpFileName,  // 文件名
        GENERIC_READ,  // 访问权限：读取
        0,  // 共享模式：不共享
        NULL,  // 安全属性：默认
        OPEN_EXISTING,  // 创建方式：仅打开已存在的文件
        FILE_ATTRIBUTE_NORMAL,  // 文件属性：普通
        NULL  // 模板文件：无
    );

    // 检查文件是否打开成功
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;  // 打开失败
    }

    // 获取文件信息
    BY_HANDLE_FILE_INFORMATION fileInfo;
    BOOL bResult = GetFileInformationByHandle(hFile, &fileInfo);

    // 如果获取成功，格式化文件信息
    if (bResult)
    {
        // 计算文件大小（低32位和高32位组合）
        DWORDLONG fileSize = ((DWORDLONG)fileInfo.nFileSizeHigh << 32) | fileInfo.nFileSizeLow;
        
        // 转换FILETIME到SYSTEMTIME（正确的时间转换）
        SYSTEMTIME creationTime, accessTime, writeTime;
        FileTimeToSystemTime(&fileInfo.ftCreationTime, &creationTime);
        FileTimeToSystemTime(&fileInfo.ftLastAccessTime, &accessTime);
        FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &writeTime);
        
        // 格式化文件信息字符串（宽字符版本）
        wsprintfW(
            lpInfoBuffer, 
            L"文件名：%s\r\n文件大小：%lld 字节\r\n创建时间：%02d:%02d:%02d %02d/%02d/%04d\r\n最后访问时间：%02d:%02d:%02d %02d/%02d/%04d\r\n最后修改时间：%02d:%02d:%02d %02d/%02d/%04d",
            lpFileName, 
            fileSize,
            // 创建时间
            creationTime.wHour, creationTime.wMinute, creationTime.wSecond,
            creationTime.wMonth, creationTime.wDay, creationTime.wYear,
            // 最后访问时间
            accessTime.wHour, accessTime.wMinute, accessTime.wSecond,
            accessTime.wMonth, accessTime.wDay, accessTime.wYear,
            // 最后修改时间
            writeTime.wHour, writeTime.wMinute, writeTime.wSecond,
            writeTime.wMonth, writeTime.wDay, writeTime.wYear
        );
    }

    // 关闭文件句柄
    CloseHandle(hFile);
    return bResult;  // 返回获取结果
}

// 总结：
// 1. 文件句柄：Win32中通过句柄来标识和操作文件，类似于文件指针
// 2. 文件操作函数：
//    - CreateFile：创建或打开文件，返回文件句柄
//    - WriteFile：向文件写入数据
//    - ReadFile：从文件读取数据
//    - CloseHandle：关闭文件句柄，释放资源
//    - GetFileInformationByHandle：获取文件的详细信息
// 3. 文件访问模式：
//    - GENERIC_READ：允许读取文件
//    - GENERIC_WRITE：允许写入文件
// 4. 文件共享模式：控制其他进程对同一文件的访问权限
// 5. 文件创建方式：
//    - CREATE_ALWAYS：总是创建新文件，如果文件已存在则覆盖
//    - OPEN_EXISTING：仅打开已存在的文件
// 6. 文件属性：FILE_ATTRIBUTE_NORMAL表示普通文件，无特殊属性
// 7. 注意事项：
//    - 所有打开的文件句柄必须调用CloseHandle关闭，否则会导致资源泄漏
//    - 操作文件前必须检查文件句柄是否有效（不等于INVALID_HANDLE_VALUE）
//    - 本示例使用Unicode编码支持，确保中文等非ASCII字符能够正确显示和处理
//    - 所有字符串处理函数都使用了宽字符版本（如CreateFileW、MessageBoxW、wsprintfW等）
//    - 读取文件时，必须为字符串添加结束符(L'\0')
//    - 文件大小可能超过32位，需要使用64位整数类型（如DWORDLONG）来存储
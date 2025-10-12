// Win32示例程序入口文件
// 功能：显示示例程序列表，允许用户选择运行特定示例

#include <iostream>
#include <string>
#include <windows.h>
#include <tchar.h>

// 示例程序信息结构体
struct ExampleInfo {
    int id;                 // 示例ID
    const char* name;       // 示例名称
    const char* description; // 示例描述
    const char* filename;   // 源文件名称
};

// 示例程序列表
ExampleInfo examples[] = {
    {1, "基础窗口", "创建一个基本的Win32窗口", "01_BasicWindow.cpp"},
    {2, "基本控件", "创建包含各种基本控件的窗口", "02_BasicControls.cpp"},
    {3, "GDI基础绘图", "使用GDI进行基础图形绘制", "03_BasicGDI.cpp"},
    {4, "文件操作", "实现基本的文件创建、读写和信息获取", "04_FileOperations.cpp"},
    {5, "菜单和对话框", "创建菜单和对话框", "05_MenuAndDialog.cpp"}
};

// 获取示例程序数量
const int EXAMPLE_COUNT = sizeof(examples) / sizeof(ExampleInfo);

// 函数声明
void ShowMenu();
bool CompileAndRunExample(int exampleId);

int main() {
    std::cout << "======================================" << std::endl;
    std::cout << "        Win32示例程序集合        " << std::endl;
    std::cout << "======================================" << std::endl;
    
    int choice = 0;
    
    while (true) {
        ShowMenu();
        
        std::cout << "请输入要运行的示例编号 (0-退出): ";
        std::cin >> choice;
        
        if (choice == 0) {
            std::cout << "感谢使用，再见！" << std::endl;
            break;
        }
        
        if (choice >= 1 && choice <= EXAMPLE_COUNT) {
            if (!CompileAndRunExample(choice)) {
                std::cout << "运行示例失败，请重试。" << std::endl;
            }
            std::cout << "\n按Enter键继续..." << std::endl;
            std::cin.ignore(); // 忽略之前的换行符
            std::cin.get();    // 等待用户按Enter键
        } else {
            std::cout << "无效的选择，请重试。" << std::endl;
        }
    }
    
    return 0;
}

// 显示示例程序菜单
void ShowMenu() {
    std::cout << "\n示例程序列表：" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    
    for (int i = 0; i < EXAMPLE_COUNT; i++) {
        std::cout << "[" << examples[i].id << "] " << examples[i].name << std::endl;
        std::cout << "    " << examples[i].description << std::endl;
    }
    
    std::cout << "[0] 退出" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
}

// 编译并运行指定的示例程序
bool CompileAndRunExample(int exampleId) {
    // 查找对应的示例信息
    ExampleInfo* example = nullptr;
    for (int i = 0; i < EXAMPLE_COUNT; i++) {
        if (examples[i].id == exampleId) {
            example = &examples[i];
            break;
        }
    }
    
    if (!example) {
        return false;
    }
    
    std::cout << "\n正在编译并运行示例：" << example->name << "..." << std::endl;
    
    // 构建编译命令
    std::string compileCmd = "g++ ";
    compileCmd += example->filename;
    compileCmd += " -o example_";
    compileCmd += std::to_string(exampleId);
    compileCmd += ".exe -mwindows";
    
    // 执行编译命令
    int compileResult = system(compileCmd.c_str());
    
    if (compileResult != 0) {
        std::cout << "编译失败！" << std::endl;
        return false;
    }
    
    std::cout << "编译成功，正在启动程序..." << std::endl;
    
    // 构建运行命令
    std::string runCmd = ".\\example_";
    runCmd += std::to_string(exampleId);
    runCmd += ".exe";
    
    // 执行运行命令
    int runResult = system(runCmd.c_str());
    
    if (runResult != 0) {
        std::cout << "程序运行异常终止。" << std::endl;
        return false;
    }
    
    return true;
}
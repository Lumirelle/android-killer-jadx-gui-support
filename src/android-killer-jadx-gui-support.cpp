// android-killer-jadx-gui-support.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "android-killer-jadx-gui-support.h"

// 获取错误信息文本
static LPWSTR GetLastErrorText(DWORD error)
{
    static WCHAR szError[128];  // 减小缓冲区大小
    FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        szError,
        ARRAYSIZE(szError),
        NULL
    );
    return szError;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    STARTUPINFO sInfo = { sizeof(sInfo) };  // 直接初始化
    PROCESS_INFORMATION pInfo = { 0 };      // 直接初始化
    WCHAR szError[128];                     // 减小缓冲区大小
    WCHAR szJadxPath[MAX_PATH];
    WCHAR szFullCmdLine[MAX_PATH * 2];
    WCHAR szExePath[MAX_PATH];

    // 获取当前程序所在目录
    GetModuleFileName(NULL, szExePath, MAX_PATH);
    
    // 往上调整三级目录，来到 android-killer 的根目录
    for (int i = 0; i < 3; i++) {
        if (WCHAR* lastBackslash = wcsrchr(szExePath, L'\\')) {
            *lastBackslash = L'\0';
        }
    }

    // 构建 jadx-gui 的完整路径
    StringCchPrintfW(szJadxPath, ARRAYSIZE(szJadxPath), 
        L"%s\\tools\\JadxGui\\jadx-gui.exe", szExePath);

    // 检查 jadx-gui 是否存在
    if (GetFileAttributesW(szJadxPath) == INVALID_FILE_ATTRIBUTES) {
        StringCchPrintfW(szError, ARRAYSIZE(szError), 
            L"找不到 jadx-gui 程序！\n路径：%s", szJadxPath);
        MessageBox(NULL, szError, L"错误", MB_OK | MB_ICONERROR);
        return -1;
    }

    // 构建完整的命令行
    StringCchPrintfW(szFullCmdLine, ARRAYSIZE(szFullCmdLine), 
        L"\"%s\" %s", szJadxPath, lpCmdLine);

    // 启动 jadx-gui
    if (CreateProcess(
        NULL,                                   // 使用完整命令行
        szFullCmdLine,                         // 完整命令行
        NULL,                                   // 进程句柄不继承
        NULL,                                   // 线程句柄不继承
        FALSE,                                  // 不继承句柄
        CREATE_NO_WINDOW,                       // 不显示窗口
        NULL,                                   // 使用父进程的环境
        NULL,                                   // 使用父进程的工作目录
        &sInfo,                                 // 启动信息
        &pInfo))                                // 进程信息
    {
        // 等待进程结束
        WaitForSingleObject(pInfo.hProcess, INFINITE);

        // 获取退出码
        DWORD exitCode;
        GetExitCodeProcess(pInfo.hProcess, &exitCode);

        // 关闭句柄
        CloseHandle(pInfo.hProcess);
        CloseHandle(pInfo.hThread);

        return exitCode;
    }
    else {
        // 获取具体的错误信息
        DWORD error = GetLastError();
        StringCchPrintfW(szError, ARRAYSIZE(szError), 
            L"jadx-gui 启动失败！\n错误代码：%d\n错误信息：%s", 
            error, GetLastErrorText(error));
        MessageBox(NULL, szError, L"错误", MB_OK | MB_ICONERROR);
        return -1;
    }
}

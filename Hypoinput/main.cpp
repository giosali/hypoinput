#include "main.h"

#include <Windows.h>
#include <cstdint>
#include <tchar.h>

// Global variables:
static TCHAR szWindowClass[] = _T("hypoinput");
static TCHAR szTitle[] = _T("Hypoinput");
HINSTANCE g_hInst = NULL;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL addNotificationIcon(HWND&);
BOOL deleteNotificationIcon(HWND&);
const uint32_t g_notifyIconId = 1;
const UINT WMAPP_NOTIFYCALLBACK = WM_APP + 1;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Prevents multiple instances of this application from running.
    HANDLE hMutex = CreateMutexEx(NULL, szWindowClass, CREATE_MUTEX_INITIAL_OWNER, MUTEX_ALL_ACCESS);
    if (!hMutex) {
        return 1;
    }

    // Maintain handle to application instance.
    g_hInst = hInstance;

    // Assign window class information.
    WNDCLASSEX wcex = { sizeof(wcex) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    // Causes the application to look crisp on displays with high DPIs.
    // This must be called before creating any HWNDs.
    SetProcessDPIAware();

    // HWND_MESSAGE causes the window to be a message-only window.
    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 450, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!hWnd) {
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseMutex(hMutex);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_COMMAND:
        break;
    case WM_CREATE:
        if (!addNotificationIcon(hWnd)) {
            return -1;
        }

        break;
    case WM_DESTROY:
        deleteNotificationIcon(hWnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

BOOL addNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
    nid.uID = g_notifyIconId;
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    nid.hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_NOTIFICATIONICON), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
    wcscpy_s(nid.szTip, L"Hypoinput v0.1.0");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is preferred
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL deleteNotificationIcon(HWND& hWnd)
{
    NOTIFYICONDATA nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uID = g_notifyIconId;
    return Shell_NotifyIcon(NIM_DELETE, &nid);
}
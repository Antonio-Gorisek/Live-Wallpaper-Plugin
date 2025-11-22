#include <windows.h>
#include <iostream>

#define DLL_EXPORT __declspec(dllexport)

// Global variable for the WorkerW window
HWND g_hWorkerW = NULL;

// Function for searching child windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    // Window behind desktop icons (WorkerW)
    // USING FindWindowExA
    HWND hDesktop = FindWindowExA(hwnd, NULL, "SHELLDLL_DefView", NULL);
    if (hDesktop != NULL) {
        g_hWorkerW = FindWindowExA(NULL, hwnd, "WorkerW", NULL);
        return FALSE;
    }
    return TRUE;
}

extern "C" {

    /**
     * @brief Sets the game window as the desktop wallpaper.
     * @param gameWindowName The name of the game window "LiveWallpaper"
     */
    DLL_EXPORT int SetGameAsWallpaper(const char* gameWindowName) {
        // Find Progman (Program Manager)
        // USING FindWindowA
        HWND hProgman = FindWindowA("Progman", NULL);
        if (hProgman == NULL) {
            return 0;
        }

        // Create WorkerW by sending WM_SETTINGCHANGE
        // This forces Progman to create the WorkerW window
        SendMessageTimeout(hProgman,
            0x052C, // WM_SETTINGCHANGE (or 0x052C)
            0,
            0,
            SMTO_NORMAL,
            1000,
            NULL);

        // Find the WorkerW window
        EnumWindows(EnumWindowsProc, NULL);

        if (g_hWorkerW == NULL) {
            return 0;
        }

        // Find the Unity game window
        // USING FindWindowA and gameWindowName
        HWND hGameWindow = FindWindowA(NULL, gameWindowName);
        if (hGameWindow == NULL) {
            return 0;
        }

        // Set WorkerW as the parent of the game window
        SetParent(hGameWindow, g_hWorkerW);

        // Remove window styles (borders, title bar)
        LONG lStyle = GetWindowLong(hGameWindow, GWL_STYLE);
        lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
        SetWindowLong(hGameWindow, GWL_STYLE, lStyle);

        // Set window size and position (fullscreen)
        RECT desktopRect;
        GetClientRect(GetDesktopWindow(), &desktopRect);

        SetWindowPos(hGameWindow,
            HWND_TOP, // Place above the WorkerW window
            desktopRect.left,
            desktopRect.top,
            desktopRect.right - desktopRect.left,
            desktopRect.bottom - desktopRect.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW);

        return 1;
    }
}

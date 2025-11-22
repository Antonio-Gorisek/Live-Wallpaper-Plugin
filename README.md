# 🖥️ Unity Live Wallpaper Plugin (C++ DLL)

This repository is used to create a C++ DLL that is used as a Unity plugin to place a Unity application directly on the Windows background as a live wallpaper.
The plugin handles WinAPI functions to adjust the Unity window for the WorkerW behind the desktop icons, effectively turning the unity build into a live animated wallpaper.

![LiveWallpaper (1)](https://github.com/user-attachments/assets/0d4f7984-98d2-4bbf-9a78-7a6e29dc9323)

## How It Works

Unity build calls the DLL via DllImport, which finds the Unity window (by name) and uses Windows functions to create a hidden WorkerW layer on the desktop. The DLL then sets the Unity window to WorkerW, removes the program frame and title bar, and resizes it to fit the desktop, setting the game window as the background on the windows.

## Easy setup
1) You can take the complete source code or directly the dll file that is already built in DLL folder `DLL\WallpaperPlugin.dll`  
2) Insert `WallpaperPlugin.dll` into the `Assets\Plugins` folder in the Unity Editor and you are done with the DLL file.  
3) You need the `WallpaperSetter.cs` script below this text. Copy the script and insert it into a GameObject in the scene.  
4) ⚠️ Don't forget to change the value of `private string gameProductName`  
5) After building and starting the project, the scene should run on the desktop as a live wallpaper.


```csharp
using System.Runtime.InteropServices; // Required for C# to call native C++ DLL functions
using UnityEngine;

public class WallpaperSetter : MonoBehaviour {

    // Tells C# that a function exists inside "WallpaperPlugin.dll"
    // C# cannot access WinAPI functions inside a C++ DLL on its own
    // This attribute defines the EXACT C++ function Unity should call
    [DllImport("WallpaperPlugin")]
    private static extern int SetGameAsWallpaper(string gameWindowName);


    // The name Windows uses as the TITLE of the Unity window (shown in the window bar).
    // It MUST match the "Product Name" in Player Settings.
    // The C++ code uses FindWindow to locate the Unity window by this exact TITLE string.
    // Once found, it allows the C++ code to take control over the Unity window.
    private string gameProductName = "YOUR PROJECT NAME";

    void Awake() {
        // Enable running in background. Without this, the game may pause when not focused.
        Application.runInBackground = true;
    }

    void Start() {
        // The Unity Win32 window does NOT fully exist at Start().
        // It takes about 0.5–1 seconds for Unity to:
        // create the actual Win32 window, assign the window title and apply borderless mode, resolution, etc.
        // If the plugin is called too early, FindWindow will return nothing.  That's why we wait 1 second :)
        Invoke(nameof(AttemptSetWallpaper), 1.0f);
    }


    void AttemptSetWallpaper() {
        // searches for the Win32 window by its name (FindWindow)
        // locates the WorkerW window (the desktop background layer)
        // sets the Unity window as a child of WorkerW (SetParent)
        // moves it behind the desktop icons
        int result = SetGameAsWallpaper(gameProductName);


        if (result == 1) {
            Debug.Log("Game successfully set as desktop wallpaper.");
        } else {
            // If the result is 0, it means the Unity window was not found or the DLL wasn't loaded correctly
            Debug.LogError("Failed to set game as desktop wallpaper. Check DLL and Win32 interop.");
        }
    }
}
```

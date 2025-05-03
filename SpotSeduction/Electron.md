# Electron
All electron applications loads similar dlls which allows for a generic solution...
Windows' `version.dll` stays loaded in the entire runtime of the process, and so that seems like a good choice to proxy.

The application folder structure is as follows:
Application/
- Updater.exe
- log...
- resources...
- \<version-number>
    - Application.exe
    - chrome_100_percent.pak
    - chrome_200_percent.pak
    - ffmpeg.dll
    - libEGL.dll
    - libEGLv2.dll
    - ...

We can proxy `C:\Windows\System32\version.dll` by dropping a malicious `version.dll` in this folder.

Electron validates the signature of the main application and main application DLL, but not the signatures of the DLLs it loads.
If in the future a mitigation against proxying Windows DLLs becomes more common, we can proxy ffmpeg.dll or some other DLL that is meant to be loaded from the same folder.

Our main dll will be placed at `%LOCALAPPDATA%\cache\xpm\405A0F3E-B30F-46EA-8DC5-DA3CF0EE1899.dll`

So we will be interferring with electron apps stored at `%LOCALAPPDATA%`, since its very easy.
For example, on Windows 11 by default msedge is located at `%LOCALAPPDATA%\`

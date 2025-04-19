# Discord Startup Writeup
Discord update `Update.exe` stored at the accessible folder `%LOCALAPPDATA%\Discord\`
By default the updater runs on startup and loads Windows' `mscoree.dll`.
Side-loading this is very easy.

## POC
Taking our SimpleDll.dll (MessageBox on up and down), we should expect it to show a popup if being placed next to Update.exe with the name `mscoree.dll`.
It will obviously not work since we don't have the proper exports, but we should expect to see the popup.

We see no popup and dll wasnt loaded
lets assume windows parses the exports and matches the required export the exe is importing...
lets quickly make a dll with that export

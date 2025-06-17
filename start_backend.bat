@echo off

REM Start each Python app in the background
start "" python -m Cherry &
start "" python -m CornCake &
start "" python -m PassionFruit &
start "" npm start --prefix PassionFruit/frontend &

REM Wait indefinitely (optional: or you could wait for a specific process or log)
:loop
timeout /t 60 > nul
goto loop

@echo off

start "" python -m Cherry &
start "" python -m CornCake &
start "" python -m PassionFruit &
start "" npm start --prefix PassionFruit/frontend &

#!/bin/bash

# Run all Python scripts in background
python -m Cherry &
python -m CornCake &
python -m PassionFruit &
npm start --prefix PassionFruit/frontend &

# Wait for all to finish
wait

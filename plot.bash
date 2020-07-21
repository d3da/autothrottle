#!/bin/bash
cd "`dirname "$0"`/plot"
source venv/bin/activate
python3 plot.py
xdg-open graph.png

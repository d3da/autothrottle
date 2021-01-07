#!/usr/bin/env python3
# 
# TODO read target temp from autothrottle config



import matplotlib.pyplot as plt
import numpy as np
from cycler import cycler

import csv

times = []
temps = []
freqs = []

target_temp = 75

# styles
#['Solarize_Light2', '_classic_test_patch', 'bmh', 'classic', 'dark_background', 'fast', 'fivethirtyeight', 'ggplot', 'grayscale', 'seaborn', 'seaborn-bright', 'seaborn-colorblind', 'seaborn-dark', 'seaborn-dark-palette', 'seaborn-darkgrid', 'seaborn-deep', 'seaborn-muted', 'seaborn-notebook', 'seaborn-paper', 'seaborn-pastel', 'seaborn-poster', 'seaborn-talk', 'seaborn-ticks', 'seaborn-white', 'seaborn-whitegrid', 'tableau-colorblind10']

#plt.style.use('seaborn-darkgrid')
plt.style.use('dark_background')

plt.rcParams["font.family"] = "serif"



with open('/tmp/autothrottle.csv') as results_file:
    csv_r = csv.reader(results_file, delimiter=',')
    line_count=0
    for row in csv_r:
        try:
            print(row[0], row[1], row[2])
            times.append(int(row[0]))
            temps.append(int(int(row[1])))
            freqs.append(int(int(row[2])))
        except:
            continue
    try:
        csv_r.__next__()
    except:
        results_file.close()

tailsize = -60*10

times = times[tailsize:]
temps = temps[tailsize:]
freqs = freqs[tailsize:]


fig, ax = plt.subplots(figsize=(8,4))
axcolor = 'r'
ax.set_title('CPU temp/freq over time')
ax.set_xlabel('t')
ax.set_ylabel('T (C)', color=axcolor)
#ax.set_ylabel('T (C)')
ax.plot(times, temps, linewidth=0.8, color=axcolor, alpha=0.8)
#ax.plot(times, temps, linewidth=0.75)
ax.tick_params(axis='y', labelcolor=axcolor)
#ax.tick_params(axis='y')

target_line = ax.axhline(target_temp, alpha=0.7, linestyle='--', linewidth=0.5, label='target temp')


ax2 = ax.twinx()
ax2color = 'c'
ax2.set_ylabel('freq (MHz)', color=ax2color)
#ax2.set_ylabel('freq (MHz)')
ax2.plot(times, freqs, linewidth=0.8, color=ax2color, alpha=0.8)
#ax2.plot(times, freqs, linewidth=0.5)
ax2.tick_params(axis='y', labelcolor=ax2color)
#ax2.tick_params(axis='y')

fig.tight_layout()
plt.legend([target_line], ['Target temp'], fontsize='small', framealpha=0.5)
plt.savefig('graph', dpi=800)
plt.show()

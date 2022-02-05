%matplotlib inline
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
from math import sqrt
linestyles = ['-', ':', '-.']
#latex figures
fig_width_pt = 600.0  # Get this from LaTeX using \showthe\columnwidth
inches_per_pt = 1.0/72.27               # Convert pt to inch
golden_mean = (sqrt(5)-1.0)/2.0         # Aesthetic ratio
fig_width = fig_width_pt*inches_per_pt  # width in inches
fig_height = fig_width*golden_mean      # height in inches
fig_size =  [fig_width,fig_height]
params = {'backend': 'ps',
          'axes.labelsize': 18,
          #'text.fontsize': 23,
          'font.weight' : 'bold',
          #'font.family': 'serif',
          #'font.serif': 'Computer Modern Roman',
          'legend.fontsize': 14,
          'xtick.labelsize': 15,
          'ytick.labelsize': 15,
          'savefig.dpi': 400,
          'text.usetex': True,
          'axes.labelweight' : 'bold',
          'figure.figsize': fig_size,
          }
plt.rcParams.update(params)



plt.figure(figsize=(5,3)) 
plt.xlim(0,20000)
plt.xlabel('Flow size (B)')
plt.ylabel("CDF")
plt.grid(which='both')
plt.plot(x2, y2, 'g', linewidth=2, label='client to server')
plt.plot(x3, y3, 'b--', linewidth=2, label='server to client')
plt.legend(loc="lower right")
plt.tight_layout()
plt.savefig('caidacdf.pdf')


import matplotlib.pyplot as plt
import pandas as pd
from math import sqrt


#Assumes time(s), flow1(mbit), flow2(mbit)...
def plot_throughput(df):
    #latex figures
    fig_width_pt = 360.0  # Get this from LaTeX using \the\columnwidth
    inches_per_pt = 1.0/72.27               # Convert pt to inch
    golden_mean = (sqrt(5)-1.0)/2.0         # Aesthetic ratio
    fig_width = fig_width_pt*inches_per_pt  # width in inches
    fig_height = fig_width*golden_mean      # height in inches
    fig_size =  [fig_width,fig_height]

    #TODO: Have not checked the effects of all these different settings
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

    #plt.xlim(left=0, right=80)
    plt.ylim(0, 7)

    plt.xlabel("Time (s)")
    plt.ylabel("Throughput (Mbit/s)")

    plt.grid(which="both")

    time_col_name = df.columns[0]
    for col in df.columns[1:]:
        #plt.plot(x2, y2, 'g', linewidth=2, label='client to server')
        #plt.plot(x3, y3, 'b--', linewidth=2, label='server to client')
        plt.plot(df[time_col_name], df[col], linewidth=2, label=col)
    
    plt.legend(loc="upper left")
    plt.tight_layout()

    plt.savefig("throughput.pdf")
    plt.show()

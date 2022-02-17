import matplotlib.pyplot as plt
import pandas as pd
from math import sqrt

fig_width_pt = 360.0  # Get this from LaTeX using \the\columnwidth

inches_per_pt = 1.0/72.27               # Convert pt to inch
golden_mean = (sqrt(5)-1.0)/2.0         # Aesthetic ratio
fig_width = fig_width_pt*inches_per_pt  # width in inches
fig_height = fig_width*golden_mean      # height in inches

#latex figures
fig_size = [fig_width,fig_height] 

#Assumes time(s), flow1(mbit), flow2(mbit)...
def plot_throughput(df):
    params = {'backend': 'ps',
      'axes.labelsize': 15,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': 14,
      'xtick.labelsize': 12,#15
      'ytick.labelsize': 12,#15
      'savefig.dpi': 400,
      'text.usetex': True,
      'axes.labelweight' : 'bold',
      'figure.figsize': fig_size,
      }

    plt.rcParams.update(params)

    plt.figure(figsize=fig_size) 

    #plt.xlim(left=0, right=80)
    plt.ylim(0, 5)

    plt.xlabel("Time (s)")
    plt.ylabel("Throughput (Mbps)")

    plt.grid(which="both")

    styles = ['solid', 'dashed', 'dashdot', 'dotted']

    time_col_name = df.columns[0]
    for (col, style) in zip(df.columns[1:], styles):
        plt.plot(df[time_col_name], df[col], linestyle=style, linewidth=2, label=col)
    
    plt.legend(loc="upper left")
    plt.tight_layout()

    plt.savefig("throughput.png")
    plt.show()

def plot_comparison_throughput(df_left, left_title, df_right, right_title):
    #plt.rcParams.update(params)
    params = {
      'axes.labelsize': 14,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': 10,
      'xtick.labelsize': 10,#15
      'ytick.labelsize': 10,#15
      'savefig.dpi': 400,
      'text.usetex': True,
      'axes.labelweight' : 'bold',
      'figure.figsize': fig_size,
      }

    plt.rcParams.update(params)

    fig, (ax_l, ax_r) = plt.subplots(1, 2, sharey=True, figsize=fig_size)
    fig.add_subplot(111, frameon=False) 
    plt.tick_params(labelcolor='none', which='both', top=False, bottom=False, left=False, right=False)

    fig.add_gridspec(1, 2, wspace=0) 

    plt.xlabel("Time (s)")
    plt.ylabel("Throughput (Mbps)")

    ax_l.set_title(left_title)
    ax_r.set_title(right_title)

    for (df, ax) in zip([df_left, df_right], [ax_l, ax_r]):
        ax.grid(which="both")

        styles = ['solid', 'dashed', 'dashdot', 'dotted']
            
        time_col_name = df.columns[0]
        for (col, style) in zip(df.columns[1:], styles):
            ax.plot(df[time_col_name], df[col], linestyle=style, linewidth=1.5, label=col)
       
        ax.legend(loc="upper left")

    fig.tight_layout()
    
    plt.savefig("throughput_comparison.png")
    plt.show()


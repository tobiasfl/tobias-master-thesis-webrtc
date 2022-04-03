import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from math import sqrt, ceil


#latex figures
fig_width_pt = 360.0  # Get this from LaTeX using \the\columnwidth

inches_per_pt = 1.0/72.27               # Convert pt to inch
golden_mean = (sqrt(5)-1.0)/2.0         # Aesthetic ratio
fig_width = fig_width_pt*inches_per_pt  # width in inches
fig_height = fig_width*golden_mean      # height in inches

fig_size = [fig_width,fig_height] 

def tex_boldify(text):
    return r"\textbf{" + text + "}"

def plot_scatter_plot(df, xlabel, ylabel):
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

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
 
    plt.grid(which="both")

    time_col_name = df.columns[0]
    for col in df.columns[1:]:
        plt.scatter(df[time_col_name], df[col], s=3, label=col)

    plt.legend(loc="upper left")
    plt.tight_layout()

    plt.savefig("scatter_plot.png")
    plt.show()

def plot_debug_line_plot(df, xlabel, ylabel):
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

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
 
    plt.grid(which="both")

    time_col_name = df.columns[0]
    for col in df.columns[1:]:
        plt.plot(df[time_col_name], df[col], label=col)

    plt.legend(loc="upper left")
    plt.tight_layout()

    plt.savefig("debug_line_plot.png")
    plt.show()

#Assumes time(s), vals1, vals2...
def plot_line_plot(df, xlabel, ylabel):
    fig_size = [fig_width,fig_height] 
    params = {'backend': 'ps',
      'axes.labelsize': 19,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': 12,
      'xtick.labelsize': 18,
      'ytick.labelsize': 18,
      'savefig.dpi': 400,
      'text.usetex': True,
      'axes.labelweight' : 'bold',
      'figure.figsize': fig_size,
      }

    plt.rcParams.update(params)

    plt.figure(figsize=fig_size) 

    time_col_name = df.columns[0]

    minx = df[time_col_name].min()
    maxx = df[time_col_name].max()
    plt.xlim(minx, maxx)

    miny = min([df[col].min() for col in df.columns[1:]])
    maxy = max([df[col].max() for col in df.columns[1:]])
    plt.ylim(miny, maxy)
    
    xticks = np.arange(minx, maxx+1, 10)
    yticks = [int(x) for x in np.arange(miny, maxy+1, 1)]

    #Hacky way to make sure ticks are also bold and correct font
    plt.xticks(xticks, [tex_boldify(str(tick)) for tick in xticks])
    plt.yticks(yticks, [tex_boldify(str(tick)) for tick in yticks])

    plt.xlabel(tex_boldify(xlabel))
    plt.ylabel(tex_boldify(ylabel))

    plt.grid(which="both")

    markers = ['o', 's','^', 'x', 'v', 'D']
    styles = ['solid', 'dashed', 'dashdot', 'dotted']

    for (col, mark, style) in zip(reversed(df.columns[1:]), markers, styles):
        if len(df.columns[1:]) > 3:
            plt.plot(df[time_col_name], df[col], markevery=2, marker=mark, linewidth=2, label=col)
        else:
            plt.plot(df[time_col_name], df[col], linestyle=style, linewidth=2, label=tex_boldify(col))
   
    legend_loc = "lower center" 
    if len(df.columns[1:]) > 2:
        plt.legend(loc=legend_loc, ncol=2)
    else:
        plt.legend(loc=legend_loc, ncol=1)
    plt.tight_layout()

    print("before savefig")
    plt.savefig("line_plot.png")
    print("before show")
    plt.show()

def plot_comparison_throughput(df_left, left_title, df_right, right_title):
    fig_size = [fig_width,fig_height*0.75] 

    params = {
      'axes.labelsize': 14,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': 8,
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

    handles = []
    labels = []
    for (df, ax) in zip([df_left, df_right], [ax_l, ax_r]):
        ax.grid(which="both")

        xlim = [0, 75]
        ylim = [0, 2.5]

        ax.set_xlim(xlim)
        ax.set_ylim(ylim)
        
        ax.set_yticks(range(ylim[0], ceil(ylim[1])))

        styles = ['solid', 'dashed', 'dashdot', 'dotted']
            
        time_col_name = df.columns[0]
        #go through sorted by name so that both plots are consistent in terms of line style and labels
        for (col, style) in zip(sorted(df.columns[1:]), styles):
            ax.plot(df[time_col_name], df[col], linestyle=style, linewidth=1.5, label=col)

        handles, labels = ax.get_legend_handles_labels()

    

    if len(labels) <= 2:
        fig.legend(handles, labels, loc='upper center', ncol=len(labels), bbox_to_anchor=(0.3,0.27)) 
    else:
        fig.legend(handles, labels, loc='upper center', ncol=2, bbox_to_anchor=(0.3,0.27)) 


    fig.tight_layout()
    
    plt.savefig("throughput_comparison.png")
    plt.show()


def plot_fourway_comparison_throughput(df_top_left, top_left_title, df_top_right, top_right_title, df_bot_left, bot_left_title, df_bot_right, bot_right_title):
    fig_size = [fig_width,fig_height*1.25] 

    params = {
      'axes.labelsize': 14,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': 8,
      'xtick.labelsize': 10,#15
      'ytick.labelsize': 10,#15
      'savefig.dpi': 400,
      'text.usetex': True,
      'axes.labelweight' : 'bold',
      'figure.figsize': fig_size,
      }

    plt.rcParams.update(params)

    fig, (((ax_t_l, ax_t_r), (ax_b_l, ax_b_r))) = plt.subplots(2, 2, sharey=True, sharex=True, figsize=fig_size)
    fig.add_subplot(111, frameon=False) 
    plt.tick_params(labelcolor='none', which='both', top=False, bottom=False, left=False, right=False)

    fig.add_gridspec(1, 2, wspace=0) 

    plt.xlabel("Time (s)")
    plt.ylabel("Throughput (Mbps)")

    ax_t_l.set_title(top_left_title)
    ax_t_r.set_title(top_right_title)
    ax_b_l.set_title(bot_left_title)
    ax_b_r.set_title(bot_right_title)

    handles = []
    labels = []
    for (df, ax) in zip([df_top_left, df_top_right, df_bot_left, df_bot_right], [ax_t_l, ax_t_r, ax_b_l, ax_b_r]):
        ax.grid(which="both")

        xlim = [0, 75]
        ylim = [0, 2.5]

        ax.set_xlim(xlim)
        ax.set_ylim(ylim)
        
        ax.set_yticks(range(ylim[0], ceil(ylim[1])))

        styles = ['solid', 'dashed', 'dashdot', 'dotted']
            
        time_col_name = df.columns[0]
        #go through sorted by name so that both plots are consistent in terms of line style and labels
        for (col, style) in zip(sorted(df.columns[1:]), styles):
            ax.plot(df[time_col_name], df[col], linestyle=style, linewidth=1.5, label=col)

        handles, labels = ax.get_legend_handles_labels()

    if len(labels) <= 2:
        fig.legend(handles, labels, loc='upper center', ncol=len(labels), bbox_to_anchor=(0.3,0.16)) 
    else:
        fig.legend(handles, labels, loc='upper center', ncol=2, bbox_to_anchor=(0.3,0.16)) 


    fig.tight_layout()
    
    plt.savefig("throughput_comparison_fourway.png")
    plt.show()


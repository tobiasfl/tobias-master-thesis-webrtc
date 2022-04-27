import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
#import seaborn as sns
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

def get_rc_params(axes_l_size, leg_f_size, xtick_l_size, ytick_l_size, fig_size):
    return {'backend': 'ps',
      'axes.labelsize': axes_l_size,
      'font.weight' : 'bold',
      'font.family': 'serif',
      'font.serif': 'Computer Modern Roman',
      'legend.fontsize': leg_f_size,
      'xtick.labelsize': xtick_l_size,
      'ytick.labelsize': ytick_l_size,
      'savefig.dpi': 400,
      'text.usetex': True,
      'axes.labelweight' : 'bold',
      'figure.figsize': fig_size,
      }

def finalize_plot(filename, legend_loc):
    plt.legend(loc=legend_loc)
    plt.tight_layout()

    plt.savefig(filename)
    plt.show()

def plot_scatter_plot(df, xlabel, ylabel):
    plt.rcParams.update(get_rc_params(15, 14, 12, 12, fig_size))

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
 
    plt.grid(which="both")

    time_col_name = df.columns[0]
    for col in df.columns[1:]:
        plt.scatter(df[time_col_name], df[col], s=1, label=col)

    finalize_plot("scatter_plot.png", "upper left")

def plot_debug_line_plot(df, xlabel, ylabel):
    plt.rcParams.update(get_rc_params(15,14,12,12,fig_size))

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
 
    plt.grid(which="both")

    time_col_name = df.columns[0]
    for col in df.columns[1:]:
        plt.plot(df[time_col_name], df[col], label=col)

    finalize_plot("debug_line_plot.png", "upper left")

# labels and list of datapoints
def plot_stacked_bar_plot(labels, ys):
    plt.rcParams.update(get_rc_params(19,12,18,18,fig_size))

    bar_width = 0.35

    fig, ax = plt.subplots(figsize=fig_size)

    labels = [tex_boldify(l) for l in labels]

    for i in range(len(ys)):
        if i == 0:
            ax.bar(labels, ys[i], bar_width)
        else:
            ax.bar(labels, ys[i], bar_width, bottom=ys[i-1])
  
    ax.set_ylabel(tex_boldify('Mbps'))
    ax.legend(loc="upper left")

    plt.savefig("stacked_bar_plot.png")
    plt.show()

#Assumes time(s), vals1, vals2...
#For pretty thesis plots
def plot_line_plot(df, xlabel, ylabel):
    plt.rcParams.update(get_rc_params(19,12,18,18,fig_size))

    plt.figure(figsize=fig_size) 

    time_col_name = df.columns[0]

    minx = df[time_col_name].min()
    maxx = df[time_col_name].max()
    maxx = 120
    plt.xlim(minx, maxx)

    miny = min([df[col].min() for col in df.columns[1:]])
    maxy = max([df[col].max() for col in df.columns[1:]])
    plt.ylim(miny, maxy)
    
    xticks = np.arange(minx, maxx+1, 30)
    #yticks = [int(x) for x in np.arange(miny, maxy+1, 1)]
    #yticks = [int(x) for x in np.arange(miny, 5.5+1, 1)]
    yticks = [float(x) for x in np.arange(miny, 3, 0.5)]

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
   
    legend_loc = "upper right" 
    if len(df.columns[1:]) > 2:
        plt.legend(loc=legend_loc, ncol=2)
    else:
        plt.legend(loc=legend_loc, ncol=1)
    plt.tight_layout()

    plt.savefig("line_plot.png")
    plt.show()

def plot_tput_and_rtt_comparison(tput_df, rtt_df):
    fig_size = [fig_width,fig_height*1.25]
    plt.rcParams.update(get_rc_params(19, 10, 18, 18, fig_size))


    fig, (ax_t, ax_b) = plt.subplots(2, figsize=fig_size, gridspec_kw={'height_ratios':[2,1]}, sharex=True)

    styles = ['solid', 'dashed', 'dashdot', 'dotted']

    plt.xlabel(tex_boldify("Time (s)"))
    
    time_min = 0
    time_max = 120
    time_ticks = np.arange(time_min, time_max+1, 20)
    plt.xticks(time_ticks, [tex_boldify(str(tick)) for tick in time_ticks])
    plt.xlim(time_min, time_max)

    #tput plot
    min_tput = 0
    max_tput = 2
    #max_tput = max([tput_df[col].max() for col in tput_df.columns[1:]])+0.5
    tput_yticks = [float(x) for x in np.arange(min_tput, max_tput+1, 0.5)]
    ax_t.set_yticks(tput_yticks, [tex_boldify(str(tick)) for tick in tput_yticks])    
    ax_t.set_ylim(min_tput, max_tput)

    time_col_name = tput_df.columns[0] 
    for (col, style) in zip(sorted(tput_df.columns[1:]), styles):
        ax_t.plot(tput_df[time_col_name], tput_df[col], linestyle=style, linewidth=1.5, label=tex_boldify(col))

    ax_t.set(ylabel=tex_boldify("mbps"))
    ax_t.grid(which="both")

    ax_t.legend(loc="lower right")


    #RTT plot
    minrtt = 100
    #maxrtt = max([rtt_df[col].max() for col in rtt_df.columns[1:]])
    maxrtt = 250
    rtt_yticks = [int(x) for x in np.arange(minrtt, maxrtt+1, 50)]
    ax_b.set_yticks(rtt_yticks, [tex_boldify(str(tick)) for tick in rtt_yticks])
    ax_b.set_ylim(minrtt, maxrtt)

    rtt_colors = ['red', 'forestgreen', 'purple']

    time_col_name = rtt_df.columns[0] 
    for (col, color) in zip(rtt_df.columns[1:], rtt_colors):
        ax_b.scatter(rtt_df[time_col_name], rtt_df[col], s=0.5, label=tex_boldify(col), color=color)
    ax_b.set(ylabel=tex_boldify("ms"))
    ax_b.grid(which="both")

    ax_b.legend(loc="upper right", ncol=2)

    fig.tight_layout()
    
    plt.savefig("tput_and_rtt.png")
    plt.show()

def plot_bar_plot():
    print("test")

#df should be format: 
def plot_heat_map(df, x_title, y_title):
    fig_size = [fig_width,fig_height] 
    
    print("test")

#Probably not gonna be used
def plot_comparison_throughput(df_left, left_title, df_right, right_title):
    plt.rcParams.update(get_rc_params(14, 8, 10, 10, [fig_width,fig_height*0.75]))

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

#Probably not gonna be used
def plot_fourway_comparison_throughput(df_top_left, top_left_title, df_top_right, top_right_title, df_bot_left, bot_left_title, df_bot_right, bot_right_title):
    plt.rcParams.update(get_rc_params(14,8,10,10,[fig_width,fig_height*1.25]))

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


def bps_to_mbps(x):
    return x / 1000000

def bp100ms_to_mbps(x):
    return x / 100000

def get_df(fn):
    df = pd.read_csv(fn) 

    for flow_col in df.columns[1:]:
       
        col = df[flow_col]
        start = flow_start(col)
        #end = len(col) - flow_start(reversed(col))

        df[flow_col] = df.apply(lambda x: bps_to_mbps(x[flow_col]), axis=1)

        #BUG: it should be -1 not -2, but -1 removes 1 too much
        df.loc[0:start-2,[flow_col]] = np.nan
    return df


def flow_start(col_it):
    index = 0
    for val in col_it:
        if val != 0:
            return index
        index += 1
    return index



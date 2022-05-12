import matplotlib.pyplot as plt
import matplotlib as mpl
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


def plot_box_plot(df):
    plt.rcParams.update(get_rc_params(16,12,10,10,fig_size))

    data = [df[col] for col in df.columns[1:]]

    fig, ax = plt.subplots(figsize=fig_size)
    ax.boxplot(data)

    print(len(df.columns))
    print(df)
    if (len(df.columns)) == 4:

        ax.set_xticks([1,2,3], [tex_boldify(x) for x in ['FSE-NG', 'Extended FSE-NG', 'FSEv2']])
    else:
        ax.set_xticks([1,2,3,4], [tex_boldify(x) for x in ['Uncoupled', 'FSE-NG', 'Extended FSE-NG', 'FSEv2']])
   
    yticks = [x for x in np.arange(100, 350, 50)]
    ax.set_yticks(yticks, [tex_boldify(str(y)) for y in yticks])

    plt.ylabel(tex_boldify("Delay(ms)"))


    fig.tight_layout()

    plt.savefig("boxplot.png")
    plt.show()

def plot_bw_differing_prios_10x10(gcc_avg_tputs, sctp_avg_tputs):
    plt.rcParams.update(get_rc_params(19,12,16,16,fig_size))

    plt.figure(figsize=fig_size) 

    plt.xlabel(tex_boldify('Priority of SCTP flow'))
    plt.ylabel(tex_boldify('Throughput(Mbps)'))

    plt.grid(which="both")

    x_data = range(0, 10)
    xticks = ['1', '0.9', '0.8', '0.7', '0.6', '0.5', '0.4', '0.3', '0.2', '0.1']
    plt.xticks(x_data, [tex_boldify(t) for t in xticks])
    plt.xlim(0, 9)

    plt.ylim(0, 2)
    yticks = [float(x) for x in np.arange(0, 2.5, 0.5)]
    plt.yticks(yticks, [tex_boldify(str(t)) for t in yticks])

    for (lst, label, style, mark) in zip([gcc_avg_tputs, sctp_avg_tputs], ['RTP', 'SCTP'], ['solid', 'dashed'], ['x', 'd']):
        plt.plot(x_data, lst, linestyle=style, linewidth=1, markevery=1, marker=mark, label=tex_boldify(label))

    finalize_plot("differing_prios_10x10", "upper left")

#Assumes time(s), vals1, vals2...
#For pretty thesis plots
def plot_line_plot(df, xlabel, ylabel):
    plt.rcParams.update(get_rc_params(19,12,18,18,fig_size))
    #plt.rcParams.update(get_rc_params(14,10,12,12,fig_size))


    plt.figure(figsize=fig_size) 

    time_col_name = df.columns[0]

    minx = df[time_col_name].min()
    maxx = df[time_col_name].max()
    maxx = 115
    plt.xlim(minx, maxx)

    miny = min([df[col].min() for col in df.columns[1:]])
    maxy = max([df[col].max() for col in df.columns[1:]])
    plt.ylim(miny, maxy)
    
    xticks = np.arange(minx, maxx+30, 30)
    #yticks = [int(x) for x in np.arange(miny, maxy+1, 1)]
    yticks = [int(x) for x in np.arange(miny, 6+1, 1)]
    #yticks = [float(x) for x in np.arange(miny, 3, 0.5)]

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
    max_tput = 5
    #max_tput = max([tput_df[col].max() for col in tput_df.columns[1:]])+0.5
    tput_yticks = [float(x) for x in np.arange(min_tput, max_tput+1, 1)]
    ax_t.set_yticks(tput_yticks, [tex_boldify(str(tick)) for tick in tput_yticks])    
    ax_t.set_ylim(min_tput, max_tput)

    time_col_name = tput_df.columns[0] 
    for (col, style) in zip(sorted(tput_df.columns[1:]), styles):
        ax_t.plot(tput_df[time_col_name], tput_df[col], linestyle=style, linewidth=1.5, label=tex_boldify(col))

    ax_t.set(ylabel=tex_boldify("Mbps"))
    ax_t.grid(which="both")

    ax_t.legend(loc="lower right", ncol=2)


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

    ax_b.legend(loc="upper left", ncol=2)

    fig.tight_layout()
    
    plt.savefig("tput_and_rtt.png")
    plt.show()

def plot_bar_plot():
    print("test")


def heatmap(data, row_labels, col_labels, ax=None,
            cbar_kw={}, cbarlabel="", **kwargs):
    """
    Create a heatmap from a numpy array and two lists of labels.

    Parameters
    ----------
    data
        A 2D numpy array of shape (M, N).
    row_labels
        A list or array of length M with the labels for the rows.
    col_labels
        A list or array of length N with the labels for the columns.
    ax
        A `matplotlib.axes.Axes` instance to which the heatmap is plotted.  If
        not provided, use current axes or create a new one.  Optional.
    cbar_kw
        A dictionary with arguments to `matplotlib.Figure.colorbar`.  Optional.
    cbarlabel
        The label for the colorbar.  Optional.
    **kwargs
        All other arguments are forwarded to `imshow`.
    """

    if not ax:
        ax = plt.gca()

    # Plot the heatmap
    im = ax.imshow(data, **kwargs)

    # Create colorbar
    cbar = ax.figure.colorbar(im, ax=ax, **cbar_kw)
    cbar.ax.set_ylabel(cbarlabel, rotation=-90, va="bottom")

    # Show all ticks and label them with the respective list entries.
    ax.set_xticks(np.arange(data.shape[1]), labels=col_labels)
    ax.set_yticks(np.arange(data.shape[0]), labels=row_labels)

    # Let the horizontal axes labeling appear on top.
    ax.tick_params(top=False, bottom=True,
                   labeltop=False, labelbottom=True)

    # Rotate the tick labels and set their alignment.
#    plt.setp(ax.get_xticklabels(), ha="right",
#             rotation_mode="anchor")

    # Turn spines off and create white grid.
    ax.spines[:].set_visible(False)

    ax.set_xticks(np.arange(data.shape[1]+1)-.5, minor=True)
    ax.set_yticks(np.arange(data.shape[0]+1)-.5, minor=True)
    ax.grid(which="minor", color="w", linestyle='-', linewidth=3)
    ax.tick_params(which="minor", bottom=False, left=False)

    return im, cbar


def annotate_heatmap(im, data=None, valfmt="{x:.2f}",
                     textcolors=("black", "white"),
                     threshold=None, **textkw):
    """
    A function to annotate a heatmap.

    Parameters
    ----------
    im
        The AxesImage to be labeled.
    data
        Data used to annotate.  If None, the image's data is used.  Optional.
    valfmt
        The format of the annotations inside the heatmap.  This should either
        use the string format method, e.g. "$ {x:.2f}", or be a
        `matplotlib.ticker.Formatter`.  Optional.
    textcolors
        A pair of colors.  The first is used for values below a threshold,
        the second for those above.  Optional.
    threshold
        Value in data units according to which the colors from textcolors are
        applied.  If None (the default) uses the middle of the colormap as
        separation.  Optional.
    **kwargs
        All other arguments are forwarded to each call to `text` used to create
        the text labels.
    """

    if not isinstance(data, (list, np.ndarray)):
        data = im.get_array()

    # Normalize the threshold to the images color range.
    if threshold is not None:
        threshold = im.norm(threshold)
    else:
        threshold = im.norm(data.max())/2.

    # Set default alignment to center, but allow it to be
    # overwritten by textkw.
    kw = dict(horizontalalignment="center",
              verticalalignment="center")
    kw.update(textkw)

    # Get the formatter in case a string is supplied
    if isinstance(valfmt, str):
        valfmt = mpl.ticker.StrMethodFormatter(valfmt)

    # Loop over the data and create a `Text` for each "pixel".
    # Change the text's color depending on the data.
    texts = []
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            kw.update(color=textcolors[int(im.norm(data[i, j]) > threshold)])
            text = im.axes.text(j, i, valfmt(data[i, j], None), **kw)
            texts.append(text)

    return texts



def plot_heat_map_2(tputs, x_ticks, y_ticks):
    plt.rcParams.update(get_rc_params(18, 10, 12, 12, [fig_width*1.50,fig_height*1.50]))
    fig, ax = plt.subplots()

    im, cbar = heatmap(tputs, y_ticks, x_ticks, ax=ax,
                       cmap="YlGn", cbarlabel="RTP avg. throughput(Mbps)")
    texts = annotate_heatmap(im, valfmt="{x:.2f}")

    plt.xlabel("Bottleneck capacity (Mbps)")
    plt.ylabel("Bottleneck queue length")


    plt.savefig("heatmap.png")
    fig.tight_layout()
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



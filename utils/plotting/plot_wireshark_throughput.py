import matplotlib.pyplot as plt
import sys
import pandas as pd
import numpy as np
from plotting import *
from math import ceil



args = sys.argv

if len(args) == 2:
    df = get_df(args[1])
    plot_line_plot(df, "Time(s)", "Throughput(Mbps)")
elif len(args) == 5:
    df1 = get_df(args[1])
    df2 = get_df(args[2])
    plot_comparison_throughput(df1, args[3], df2, args[4]) 
elif len(args) == 9:
    df1 = get_df(args[1])
    df2 = get_df(args[2])
    df3 = get_df(args[3])
    df4 = get_df(args[4])
    plot_fourway_comparison_throughput(df1, args[5], df2, args[6], df3, args[7], df4, args[8])
else:
    print(f'Usage: {args[0]} <csv file> [csv file 2] [title 1] [title 2]')
    sys.exit(1)

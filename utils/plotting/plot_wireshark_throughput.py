import matplotlib.pyplot as plt
import sys
import pandas as pd
import numpy as np
from plotting import *
from math import ceil

def get_df(fn):
    df = pd.read_csv(fn) 

    for flow_col in df.columns[1:]:
       
        col = df[flow_col]
        start = flow_start(col)
        end = len(col) - flow_start(reversed(col))

        df[flow_col] = df.apply(lambda x: x[flow_col] / 1000000, axis=1)

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



args = sys.argv

if len(args) == 2:
    df = get_df(args[1])
    plot_throughput(df)
elif len(args) == 5:
    df1 = get_df(args[1])
    df2 = get_df(args[2])
    plot_comparison_throughput(df1, args[3], df2, args[4]) 
else:
    print(f'Usage: {args[0]} <csv file> [csv file 2] [title 1] [title 2]')
    sys.exit(1)

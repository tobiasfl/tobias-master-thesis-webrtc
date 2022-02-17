import matplotlib.pyplot as plt
import sys
import pandas as pd
import numpy as np
from plotting import plot_throughput
from math import ceil

args = sys.argv

if len(args) != 2:
    print(f'Usage: {args[0]} <csv file>')
    sys.exit(1)

def plot():
    df = pd.read_csv(args[1]) 

    for flow_col in df.columns[1:]:
       
        col = df[flow_col]
        start = flow_start(col)
        end = len(col) - flow_start(reversed(col))
        print("end", end) 

        df[flow_col] = df.apply(lambda x: x[flow_col] / 100000, axis=1)
        #BUG: it should be -1 not -2, but -1 removes 1 too much
        df.loc[0:start-2,[flow_col]] = np.nan
        df.loc[end:len(col),[flow_col]] = np.nan

    plot_throughput(df)


def flow_start(col_it):
    index = 0
    for val in col_it:
        if val != 0:
            return index
        index += 1
    return index


plot()

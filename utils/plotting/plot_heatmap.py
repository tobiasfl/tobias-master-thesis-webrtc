from plotting import *
import numpy as np

import sys
def main():
    #Should get data from a txt file
    
    mbps3 = [0.155, 0.174, 0.345, 0.708]
    mbps4 = [0.441, 0.541, 0.912, 1.237]
    mbps5 = [0.903, 1.098, 1.682, 1.702]
    mbps6 = [1.475, 1.853, 1.893, 1.966]
    mbps7 = [2.112, 1.942, 2.048, 2.146]
    mbps8 = [2.385, 2.312, 2.286, 2.34]
    mbps9 = [2.366, 2.376, 2.387, 2.412]
    mbps10 = [2.387, 2.399, 2.442, 2.432]
    uncoupled_tputs = np.array([mbps3, mbps4, mbps5, mbps6, mbps7, mbps8, mbps9, mbps10]).transpose()

    yticks = ['50 ms', '100 ms', '200 ms', '300 ms']
    xticks = [str(x) for x in range(3,11)]
    #plot_heat_map_2(uncoupled_tputs, xticks, yticks) 


    mbps3 = [1.329, 1.37, 1.345, 1.288]
    mbps4 = [1.707, 1.768, 1.683, 1.725]
    mbps5 = [1.977, 1.958, 2.048, 2.025]
    mbps6 = [2.092, 2.187, 2.208, 2.189]
    mbps7 = [2.192, 2.226, 2.238, 2.192]
    coupled_tputs = np.array([mbps3, mbps4, mbps5, mbps6, mbps7]).transpose()
    xticks = [str(x) for x in range(3,8)]
    print(xticks)

    plot_heat_map_2(coupled_tputs, xticks, yticks) 


main()

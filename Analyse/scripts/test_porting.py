#from ..core import processing as proc
from Analyse.core import processing as proc
from Analyse.core import plotting as pl
from Analyse.core import misc as misc

import os
import matplotlib.pyplot as plt

def main():

    # root file location
    PATH = str(misc.bash_var("WD_STORAGE")) + "John/testing/RUN000004/PMT/Nominal/"
    file = "wave_1.dat.root"

    # port root file (0th event) and plot it
    vals = proc.port_event(file, PATH, x_data = True)
    plt.plot(vals[0],vals[1])
    plt.xlabel("Sample Time (ns)", fontsize = 17)
    plt.ylabel("ADC Value", fontsize = 17)
    plt.xticks(fontsize=16)
    plt.yticks(fontsize=16)
    plt.title(str(file) + " event " + str(0), fontsize = 22)
    plt.show() 

main()
import lecroyparser as parse
import numpy as np
import matplotlib.pyplot as plt
from os import walk
from os.path import exists
from os import mkdir
from scipy import stats

from Analyse.core import plotting as pl


def main():
    '''
    Uses the plot_ADC function from plotting
    to produce histogram from npy file   
    '''

    dir = 'storage/John/testing/'
    end_path = '/PMT/analysed_data/ADC_data.npy'
    RUN_NO = 'RUN000036'
    

    adc_data = np.load(dir + RUN_NO + end_path)
    print("Loading {} ADC data...".format(RUN_NO))

    # plot
    pl.ADC_plot(adc_data, bins = 100, run_no = RUN_NO)


main()
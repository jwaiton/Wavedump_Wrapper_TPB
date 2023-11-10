'''
    John Waiton - 2023
        Currently this script will take a folder (PATH) and output the lecroy TRC files as a charge histogram with minimalist baseline subtraction.
        Will be further developed and better commented if need-be
        --- THIS VERSION IS FOR TESTING MOVING ALL THE GUTS OF THIS CODE TO /CORE/ ---

'''

import lecroyparser as parse
import numpy as np
import matplotlib.pyplot as plt
from os import walk
from os.path import exists
from os import mkdir
from scipy import stats

from Analyse.core import processing as proc
from Analyse.core import plotting as pl
from Analyse.core import misc as misc

def ADC_plot(ADCs, bins = 100,run_no = -1):
    '''
    plot charge histogram of event with ADCs along x and events along y
    '''

    # check
    if run_no == -1:
        "Input a run_number before plotting!"
        return

    x_label = "ADC counts"
    y_label = "Counts"
    plt.hist(ADCs, bins)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title("Charge histogram")
    plt.yscale('log')
    plt.savefig(output_dir + "RUN_" + str(run_no) + "/ADC_plot.png")
    plt.show()


def main():

    #plot_signal_event(test_event)

    run_no = input("Run Number: ")

    # make directory if it doesnt exist in output directory
    if exists(output_dir+"RUN_" + str(run_no)):
        print("Directory exists! Overwriting previous data...")
    else:
        mkdir(output_dir+"RUN_" + str(run_no))


    # collect then save data
    data = proc.collate_ADC_data(PATH)
    np.save(output_dir + "RUN_" + str(run_no) + "/ADC_data",np.array(data))


    ## load data
    #data = np.load(output_dir+"RUN_" + str(run_no) + '/ADC_data.npy')

    # hist show data
    ADC_plot(data, bins = 60, run_no = run_no)


    print("Job done!")

if __name__ == "__main__":

    #PATH = "SR_testing/SR_testing_500NS_5kS/"
    PATH = "../../../../../media/e78368jw/T7/33_3mV_PMT1003_meantime/"
    test_event = "C1--PMT-test_calibration_long--00000.trc"
    output_dir = "output/"
    main()

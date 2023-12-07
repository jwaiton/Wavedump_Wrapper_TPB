import lecroyparser as parse
import numpy as np
import matplotlib.pyplot as plt
from os import walk
from os.path import exists
from os import mkdir
from scipy import stats


# script that includes all the basic plotting functions required in the outside scripts



def plot_signal_event(event_name, PATH):
    '''
    plot events that appear to have signal in them (y value much larger than the baseline)
    '''
    data = parse.ScopeData(PATH+str(event_name))
    x_vals = np.linspace(0,len(data.x), dtype = int, num = len(data.x), endpoint = True)
    #plt.plot(data.x, data.y)

    # flip
    plt.plot(x_vals, -data.y)
    plt.title(str(event_name))
    plt.show()


def plot_single(test_event, PATH):
    '''
    Plot raw output
    '''

    data = port_event(test_event, PATH)
    print(data)
    plt.plot(data.x, data.y)
    plt.show()

    return 0


def ADC_plot(ADCs, bins = 100,run_no = -1, output_dir = False):
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
    if (output_dir == True):
        plt.savefig(output_dir + "RUN_" + str(run_no) + "/ADC_plot.png")
    plt.show()


def plot_waveform(file_path = 'NA', data = 'NA', time = (100,90), log_plot = False):
    '''
        Relatively obtuse waveform plotter, modify to your own tastes
    '''
    if (file_path != 'NA'):
        new_data = np.load(file_path)
        print("Processing {}".format(file_path))
    elif (data != 'NA'):
        print("Processing data...")
        new_data = data
    else:
        print("Please provide input")
    
    #data = np.load(file_path)

    print(len(new_data))
    # figure out how much data to strip
    div_frac = (time[1]/time[0])
    # then strip it!
    strip_val = len(new_data) - int(len(new_data)*div_frac)
    new_data = new_data[strip_val:]
    print(len(new_data))
    #newerdata = newdata[newdata > -1000]
    #newdata = data[(data>0)]
    bins = 100
    #plt.hist(newdata, bins)

    # subtract median
    new_data = np.abs(new_data - np.median(new_data))

    if log_plot == False:
        # CURRENT SETUP FOR PRODUCING WAVEFORM PLOTS ACROSS X TIMESCALE, set range to 0,1000 for 1ms.
        plt.plot(np.linspace(0,99.605,num = len(new_data),endpoint = True),new_data)
    elif log_plot == True:
        plt.plot(np.logspace(np.log10(0.001), np.log10(99.606), num = len(new_data), endpoint = True), new_data)
    
    plt.yscale('log')
    plt.xscale('log')
    plt.xlabel('Time [us]')
    plt.ylabel('Amplitude [a.u.]')
    plt.show()

if __name__ == "__main__":
    file_path = "output_waveforms/RUN_34/ADC_data.npy"
    plot_waveform(file_path = file_path)

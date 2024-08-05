import numpy as np
import matplotlib.pyplot as plt
from os import walk
import uproot
from os.path import exists
from os import mkdir
from scipy import stats
import awkward as ak
from tqdm import tqdm
import pandas as pd

import h5py

'''
(jwaiton 2024)
Messy collection of processing functions for TPB project
'''

def is_root(file):
    '''
    checks if file is root and therefore is formatted differently

        Parameters:
                file (string): file in question

        Returns:
                isR (bool): Whether or not file is root (true) or not (false)
    '''
    if file[-5:] == ('.root'):
        return True
    else:
        return False

def port_event(event_name, PATH, x_data = False, event = 0):
    '''
    collect data for a singular lecroy trc file
    or a root file

    :param: x_data      False implies that it isn't required, true implies that time data is required
    '''
    # ive moved this in here to see if it avoids issues.
    # This whole function should be moved to a lecroy related file
    import lecroyparser as parse
    # if a root file, return first event corresponding to 'event'
    if is_root(str(event_name)):
        print("Root file!")
        tree = uproot.open(PATH+str(event_name))["T;19"]
        branches = tree.arrays()
        #print(branches['ADC'])

        # how long between data taken
        timegate = 2
        # length of event
        eventno = len(branches['ADC'][0])
        time = []
        # Creating list for sample times that are 2ns intervals, 150 samples
        for i in range(eventno):
            time.append(i*timegate)

        x = time
        y = ak.to_list(branches['ADC'][event])

        if (x_data == False):
            return y
        if (x_data == True):
            return (x, y)
        #plt.plot(time,branches['ADC'][event])
        #plt.xlabel("Sample Time (ns)", fontsize = 17)
        #plt.ylabel("ADC Value", fontsize = 17)
        #plt.xticks(fontsize=16)
        #plt.yticks(fontsize=16)
        #plt.title(str(filename) + " event " + str(event), fontsize = 22)
        #plt.show()
    else:
        data = parse.ScopeData(PATH+str(event_name))
        #path = PATH+str(event_name)
        #contents = open(path, 'rb').read()
        #data = parse.ScopeData(data=contents)
        ####print("Initial data value: {}".format(data.y[25]))
        if (x_data == False):
            return data.y
        elif (x_data == True):
            return data.x, data.y



def integrate(y_data):
    '''
    collect the integral across an event by summing y components
    '''
    ####print("Baseline subtracted value: {}".format(y_data[25]))
    int_tot = np.sum(y_data)
    return(int_tot)



def integrate_range(y_data, window = 0, debug = False):
    '''
    Selects range to integrate over based on largest value within the event. 
    If no window is given, chooses 10.

    Perhaps make 'central value' choice an option later rather than largest value? undecided

    Also provides height and sigma value of the largest peak.

    So returns:
        (integration value, sigma value, and height)
    '''
        # if window != 0, find index of largest event in data
    if (window == 0 ):
        window = 10
        

    # find largest event
    peak_index = np.argmax(y_data)
    y_max = y_data[peak_index] # fixing issue with debug counting

    # safety check to make sure index overflow doesn't occur
    # if overflow, set window to centre
    if ((peak_index-window) < 0)  or (peak_index+window > len(y_data)):
        if debug == True:
            print("Window overlapping with array edges, setting to centre...")

        peak_index = len(y_data)//2
    # take samples around this relating to the window and save them for integration
    y_data = y_data[peak_index-window:peak_index+window]
    
    if debug == True:
        print("Max value {:.4g} found at index {:.4g}. Integrating...".format(y_max, peak_index))

    time = []
    for i in range(len(y_data)):
        time.append(i*2)
    
        # mean of waveform
    sumb = np.sum(y_data)
    mean = np.sum((y_data*time)/sumb)

    
    sigma = np.sqrt(np.sum((np.square(mean - time)*y_data)/sumb))
    
    # safe check
    if np.isnan(sigma):
        print("Setting NaN to -9999")
        sigma = -9999



    return(integrate(y_data), sigma,  y_max)

def raw_read(event_name, PATH, number = 0):
    '''
    Reads out all the raw wave forms to a variable
    '''

    tree = uproot.open(PATH+str(event_name))
    print("File found, {} branches available: {}".format(len(tree.keys()), tree.keys()))

    y_vals = []

    # just for initial setting of loops
    key = tree.keys()[0]
    br = tree[key]
    branches = br.arrays()

    # set number of events collected
    if (number == 0):
        Q = len(branches['ADC'])
    else:
        Q = number

    for i in range(len(tree.keys())):
        print("({}/{})".format(i+1, len(tree.keys())))
        
        key = tree.keys()[i]
        br = tree[key]
        branches = br.arrays()
        # asumming CAEN 1730B digitiser is still in use, 2ns sampling rate
        eventno = len(branches['ADC'][0])
        timegate = 2
        time = []
        # collecting time information
        for i in range(eventno):
            time.append(i*timegate)

        # collecting waveforms
        for i in tqdm(range(Q)):
            # scanning over all events
            a = ak.to_numpy(branches['ADC'][i])
            y_vals.append(a)

        # this is done to make sure we get an output, as it keeps breaking for multiple keys
        break
    return (time, y_vals)

def cook_raw(event_name, PATH):
    '''
    Collect and process all the ADC values across individual events recursively
    This function is made to work with CAEN wavedump data which has been
    converted into ROOT files

    Name inspired by Wavedump's C equivalents
    '''

    tree = uproot.open(PATH+str(event_name))
    print("File found, {} branches available: {}".format(len(tree.keys()), tree.keys()))
    

    ADC_list = []

    for i in range(len(tree.keys())):
        print("({}/{})".format(i+1, len(tree.keys())))
        # take specific branch
        key = tree.keys()[i]
        br = tree[key]
        branches = br.arrays()
        # asumming CAEN 1730B digitiser is still in use, 2ns sampling rate
        eventno = len(branches['ADC'][0])
        timegate = 2
        time = []
        # in case we need the time for whatever reason
        for i in range(eventno):
            time.append(i*timegate)

        
        # lets try this recursive method first, to see how fast it is
        for i in tqdm(range(len(branches['ADC']))):
            # scanning over all events
            a = ak.to_numpy(branches['ADC'][i])

            # flip to positive
            a = -a
            b = subtract_baseline(a, type = 'median')
            c = integrate_range(b, window = 10, debug=False)

            #if (c < -500):
            #    print(c)
            #    plt.plot(plot_numbers,a)
            #    plt.show()

            ADC_list += (c[0]),
    return ADC_list

def collate_ADC_data(PATH):
    '''
    Collect and process all the ADC values across individual events recursively.
    This function is made to work with lecroy oscilloscope data
    needs to be made workable for CAEN and lecroy or scrapped
    '''
    # collect filenames
    filenames = next(walk(PATH), (None, None, []))[2]
    print(filenames[1])
    print("Number of files: {}".format(len(filenames)))

    file_length = len(filenames)
    plot_numbers = np.linspace(0,2000002, dtype = int, num = 2000002, endpoint = True)
    display_vals = np.linspace(0,file_length, dtype = int, num = 25 )

    ADC_list = []
    for i in range(file_length):
        # integrate y axis of each event and append to ADC_list

        # breaking it down into constituent components for testing
        #plot_signal_event("C1--PMT-test_calibration_long--01934.trc")
        #print(filenames[i])
        try:
            a = port_event(filenames[i], PATH)
            # flip to positive
            a = -a
            #print(a)

            b = subtract_baseline(a, type = 'median')
            #print(b)
            c = integrate_range(b, window = 10, debug=False)
            #if (c < -500):
            #    print(c)
            #    plt.plot(plot_numbers,a)
            #    plt.show()

            ADC_list += (c[0]),

            # print when used
            if i in display_vals:
                # print progress
                print("{:.1f}% complete".format((i/len(filenames))*100))


        except Exception as e:
            print(e)
            pass
    return ADC_list

def subtract_baseline(y_data, type = 'median'):
    '''
    remove the pedestal in singular events (quickly!)
    '''


    # convert y_data to numpy array for your own sanity
    y_data = np.array(y_data)

    # MEAN METHOD
    # add all ADC values and divide by length (rough), also remove negatives
    if (type=='mean'):
        total = (np.sum(y_data)/len(y_data))
    # MODE METHOD
    elif (type=='mode'):
        value, counts = np.unique(y_data, return_counts=True)
        m = counts.argmax()
        # counteracting mode being stupid
        #if counts[m] == 1:
        #    print("Only one count of this value, please use a different method! (Mode sucks Brais >:( ))")
        #else:
        #    total = value[m]
        total = value[m]
        ## SCIPY IS SLOW!
        ##return (stats.mode(y_data))
    # MEDIAN METHOD
    elif (type=='median'):
        total = np.median(y_data)
    else:
        print("Please input a baseline method, exiting...")
        return 0

    # return values subtracted
    return y_data - total


def read_raw_h5(PATH, save_h5 = False, cook = False, verbose = False, print_mod = 0):
    '''
    Read in .dat data and output as a pandas array
    
    Has flag for saving this is a h5.

    Args:
        PATH        (str)       :       File path of interest
        save_h5     (bool)      :       Flag for saving data
        cook        (bool)      :       Flag for whether data should be cooked
        verbose     (bool)      :       Flag for outputting information
        print_mod   (int)       :       Print modifier

    Returns:
        data        (int 2D array) :       2D array of events
                                            First element defines event
                                            Second element defines ADC value
    ''' 

    # Makeup of the header (array[n]) where n is:
    # 0 - event size (ns in our case, with extra 24 samples)
    # 1 - board ID
    # 2 - pattern (not sure exactly what this means)
    # 3 - board channel
    # 4 - event counter
    # 5 - Time-tag for the trigger

    # Output data is a collection of ints defined in size
    # by (event size - 24) // 2

    file = open(PATH, 'rb')
    data = []

    print("File open! Processing...")
    # Collect data, while true loops are always dangerous but lets ignore that here :)
    while (True):

    # take the header information from the file (first 6 elements)
        array = np.fromfile(file, dtype='i', count=6)

        # breaking condition
        if len(array) == 0:
            print("Processing finished! Saving...")
            break
        
        # printing events
        if (array[4] % int(print_mod) == 0):
            print("Event {}".format(array[4]))
        
        # verbose check
        if (verbose == True):
            array_tag = ['event size (ns)', 'board ID', 'pattern', 'board channel', 'event counter', 'trigger tag']
            for i in range(len(array)):
                print("{}: {}".format(array_tag[i], array[i]))
        


        # alter event size to the samples
        array[0] = array[0] - 24

        # collect event
        event_size = array[0] // 2

        int16bit = np.dtype('<H')
        data.append(np.fromfile(file, dtype=int16bit, count=event_size))
    
    if (save_h5 == True):
        print("Saving raw waveforms...")
        # change path to dump the h5 file where
        # the .dat file is
        directory = PATH[:-3] + "h5"

        h5f = h5py.File(directory, 'w')
        h5f.create_dataset('pmtrw', data=data)
        h5f.close()
    else:
        directory = ""

    # if cook == True
    if (cook == True):
        print("Cooking data...")
        ADC_data = cook_raw_h5(data,directory)



    return data



def read_ascii_h5(PATH, save_h5 = False, cook = False, verbose = False, print_mod = 0):
    '''
    Read in .txt data and output as a pandas array.
    Only accepts the ascii format output by Wavedump2
    
    Has flag for saving this is a h5.

    Args:
        PATH        (str)       :       File path of interest
        save_h5     (bool)      :       Flag for saving data
        cook        (bool)      :       Flag for whether data should be cooked
        verbose     (bool)      :       Flag for outputting information
        print_mod   (int)       :       Print modifier

    Returns:
        data        (int 2D array) :       2D array of events
                                            First element defines event
                                            Second element defines ADC value
    ''' 

    # this should be re-written to work iteratively.
    with open(PATH, mode = 'rb') as file:
        data_txt = file.read()
    # split them by 'Event', first element is empty so ignored
    data_txt_full = (data_txt.decode('ascii').split('Event n.'))[1:]


    if verbose == True:
        print("Limited functionality because I'm lazy.\n This should display the first events details.")
        print(data_txt[:63].decode('ascii'))

    # remove majority of nonsense values
    stripped_data = [item.split(' Sample')[1].split('\n', 1)[1] for item in data_txt_full]

    data = [np.array(item.split('	')[:-1], dtype = int) for item in stripped_data]
    
    if (save_h5 == True):
        print("Saving raw waveforms...")
        # change path to dump the h5 file where
        # the .dat file is
        directory = PATH[:-3] + "h5"

        h5f = h5py.File(directory, 'w')
        h5f.create_dataset('pmtrw', data=data)
        h5f.close()
    else:
        directory = ""

        # if cook == True
    if (cook == True):
        print("Cooking data...")
        ADC_data = cook_raw_h5(data,directory)


    return data
    


def cook_raw_h5(data, directory = "", FIT = False):
    '''
    Function to produce charge histogram and relevant waveform information
    and output to h5 file

    Args:
        data        (array)     :       Waveform data, an array with each element being a waveform
                                        (collection of integers, usually)

        directory   (str)       :       Directory for outputting (if one exists)
                                        This acts as a flag to determine if the ADC values
                                        should be output to a file or not.
                                        "" means no, anything else means yes

        FIT         (bool)      :       Flag for producing gaussian fit

    Returns:
        h5_file
        plots
    '''

    # information lists
    ADC_list = []
    sigma_list = []
    height_list = []

    # subtracted waveform list
    subwf = []
    print("Integrating: [1/2]")
    # we are assuming that the time windows stay consistent. Currently its 2ns windows

    for i in tqdm(range(len(data))):

        a = data[i]
        # flip to positive
        a = -a
        b = subtract_baseline(a, type = 'median')
        # add subtracted baseline data to subwf dataframe
        subwf.append(b)


        # adc and height list calc
        c = integrate_range(b, window = 10, debug=False)
        ADC_list += (c[0]),
        sigma_list += (c[1]),
        height_list += (c[2]),

        

    print("ADC min/max values:")
    print("Min: {}      Max: {}".format(min(ADC_list), max(ADC_list)))


    print("Producing ADC plots")
    plt.hist(ADC_list, bins = 200)
    plt.yscale('log')
    plt.title("Charge histogram plot")
    plt.xlabel('ADC values')
    plt.ylabel('Counts')
    plt.savefig('../Output/Q_HIST.png')
    plt.show()

    

    # create summary dataframe
    sumdf = pd.DataFrame({'ADC': ADC_list, 'sigma': sigma_list, 'heights': height_list})
    ## check whether or not to write
    if (directory != ""):
        print("Writing data to {}".format(directory))
        # read/write if exists
        h5f = h5py.File(directory, 'a')
        # create subtracted waveform data
        num_rows = len(subwf)
        num_cols = len(subwf[0])

        # how many events to write at once
        batch_size = 100
        chunk_size = (1, num_cols)


        dset = h5f.create_dataset('subwf', shape = (num_rows, num_cols), chunks = chunk_size, compression = 'gzip')
        
        for i in tqdm(range(0, num_rows, batch_size)):
            batched_data = subwf[i:i+batch_size]
            dset[i:i+batch_size, :] = batched_data
        h5f.close()
        print("Dataset created, creating summary table")
        # give ADC values
        sumdf.to_hdf(directory, key = 'summary', mode = 'r+', format='fixed', data_columns = True)



        #for i, value in enumerate(ADC_list):
        #    dataset[i][0] = value

        

    return ADC_list


### Example usage

def main():

    #plot_signal_event(test_event)

    run_no = input("Run Number: ")

    # make directory if it doesnt exist in output directory
    if exists(output_dir+"RUN_" + str(run_no)):
        print("Directory exists! Overwriting previous data...")
    else:
        mkdir(output_dir+"RUN_" + str(run_no))


    # collect then save data
    data = collate_ADC_data(PATH)
    np.save(output_dir + "RUN_" + str(run_no) + "/ADC_data",np.array(data))


    ## load data
    #data = np.load(output_dir+"RUN_" + str(run_no) + '/ADC_data.npy')

    # hist show data
    ADC_plot(data, bins = 60, run_no = run_no)

if __name__ == "__main__":

    #PATH = "SR_testing/SR_testing_500NS_5kS/"
    PATH = "alpha_test_100us/56mV_trig_100us_dark8hrs/"
    test_event = "C1--PMT-test_calibration_long--00000.trc"
    output_dir = "output/"
    main()

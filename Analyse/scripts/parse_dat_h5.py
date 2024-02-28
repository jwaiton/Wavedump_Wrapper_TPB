'''
    John Waiton - 2024
        This script is the basis for which files will be converted to h5 files
        rather than the standard ROOT files

        At the moment it currently has all the working for this system, as it's
        currently unfinished

'''

import numpy as np
import h5py
import pandas as pd
import matplotlib.pyplot as plt

def dat_h5(file_path):
    '''
    Convert .dat file to .h5 with h5py
    '''

    file = open(file_path + "wave_6.dat", 'rb')

    array = np.fromfile(file, dtype='i', count=6)
    print(array)

    # event is done by ns, so 150 samples across 300 ns
    # here we have 324, so you need -24, then divide by 2 to get
    # the true number of events
    event_size = (array[0] - 24) // 2

    int16bit = np.dtype('<H')

    data = np.fromfile(file, dtype=int16bit, count=event_size)

    x = np.linspace(0,len(data), len(data))

    print(data)

    plt.plot(x, data)
    plt.show()


    # load h5 data
    
    # take the first 6 components from the event-header
    

    ## load h5 data
    #data = np.loadtxt(file_path + "wave_6.dat")
    #data = pd.read_csv(file_path + "wave_6.dat")

    # datatype of unsigned 16bit integers 
    ###data = np.fromfile(file, dtype=dtype('<H'), count=)

    #print(data)

dat_h5('/home/e78368jw/Documents/TPB_Project/Wavedump_Wrapper_TPB/storage/John/testing/RUN000051/PMT/Nominal/')
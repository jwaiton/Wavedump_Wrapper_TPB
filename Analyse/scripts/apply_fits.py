'''
John Waiton - 27.09.2023
    Script that utilises other scripts to produce 2 - gaussian fits to charge histogram .npy data

'''
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.optimize import curve_fit

from Analyse.core import fitting as fits
from Analyse.core import misc as misc

'''
Temporary storage for apriori while i figure out how to write this more neatly
Lecroy apriori for fixed mu:

            a_prio.append([370, 0.02])                          # one gaussian
            a_prio.append([2986, x[p], 0.02, 500, 0.01]) 

'''


def apply_fits(file_path = 'N/A', prom = 0, n_gauss = 0, fnc = 'none', verbose = False, fix = True):

    # defaults for ease of use
    storage = str(misc.bash_var("WD_STORAGE"))
    path = str("John/testing/RUN000007/PMT/analysed_data/")
    file_path = storage + path + str("ADC_data.npy")   # file path
    prom = 100                                  # prominence of peaks
    n_gauss = 2                                 # number of gaussians
    fnc = [fits.gauss_1, fits.gauss_2]                    # the separate functions
    verbose = False                             # verbose 
    fix = True                                  # fixed first mu value for gaussian

    n_g = n_gauss-1      

    ################################## INITIAL PROCESSING ##################################

    # produce data
    x, y  = fits.produce_data_points(file_path, bin_no = 150, plot = verbose)
    
    # make them numpy arrays for easier manipulation
    x = np.array(x)
    y = np.array(y)

    # find peaks
    p, v = fits.find_PV(x, y, plot = True)
    
    # de-numpy these values (1,1 arrays)
    p = p[0]
    v = v[0]

    # collect a_prios for fitting
    a_prio = []
    # and labels for fitting parameters
    labels = []

    ################################## SETTING PARAMETERS FOR FIXED/NON-FIXED SCENARIOS ##################

    if (fix == False):

        labels.append(["A1", "mu1", "sigma1"])
        labels.append(["A1", "mu1", "sigma1", "A2", "mu2", "sigma2"])

        a_prio.append([370, x[p], 0.02])                    # one gaussian
        a_prio.append([370, x[p], 0.02, 100, x[p]*2, 0.02]) # two gaussians

        mu_fix = 0                                              # set a value for mu to zero
    else: # fixed case, ignore mu
        if (n_gauss == 2):                      # CURRENTLY ONLY WORKS FOR 2 GAUSSIANS

            labels.append(["A1", "sigma1"])
            labels.append(["A1", "mu1", "sigma1", "A2", "sigma2"])

            a_prio.append([36261, 0.06])                          # one gaussian
            a_prio.append([36261, x[p], 0.06, 3000, 0.03])               # two gaussians

            print("mu fixed to: {}".format(x[p]))


            mu_fix = x[p]
        else:
            print("Functionality for fixed mu only available for 2 gaussian model!")
            exit()

    ################################## DATA PROCESSING, FIT PRODUCTION ##################################


    x_trim, y_trim = fits.trim_data(x, y, p, v, n_gauss, plot = True)




    popt, pcov = fits.create_fit(fnc[n_g], x_trim, y_trim, p, v, n_gauss, a_prio[n_g], mu = mu_fix)

    #print popt wrt labels
    fits.print_parameters(popt,pcov, labels[n_g])

    # plot fit
    fits.plot_fit(fnc[n_g], x, y, x_trim, y_trim, popt, mu = mu_fix, n_ga = n_gauss)













if __name__ == "__main__":

    apply_fits()
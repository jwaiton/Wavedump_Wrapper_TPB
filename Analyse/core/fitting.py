import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks
from scipy.optimize import curve_fit

# script that will take data and apply a fitting function to it. Then spit out the popt, pcov.

def gauss_1(x, A, mu, sigma):
    '''
    Gaussian function as per https://en.wikipedia.org/wiki/Gaussian_function
    
    :param x: X values
    :param A: Amplitude
    :param mu: Mean value 
    :param sigma: Variance

    :return: Y values wrt x for Gaussian function
    '''

    g = A * np.exp(-((x - mu)**2)/(2*(sigma**2)))

    return g


def gauss_2(x, A1, mu1, sigma1, A2, mu2, sigma2):
    '''
    2 Gaussian functions applied additively

    :param x: X values
    :param A1: Amplitude of 1st Gaussian
    :param mu1: Mean value of 1st Gaussian 
    :param sigma1: Variance of 1st Gaussian

    :param A2: Amplitude of 2nd Gaussian
    :param mu2: Mean value of 2nd Gaussian 
    :param sigma2: Variance of 2nd Gaussian

    :return: Y values wrt x for double Gaussian function   
    '''

    g1 = A1 * np.exp(-((x - mu1)**2)/(2*(sigma1**2)))
    g2 = A2 * np.exp(-((x - mu2)**2)/(2*(sigma2**2)))

    return (g1 + g2)

def fit_func(function, x, y, p0):
    '''
    Function that separates the ugly parameters of curve_fit from the main codeline to reduce clutter.
    Preferably used for singular gaussian fits

    :param function: Input function
    :param x: X values
    :param y: Y values
    :param p0: A priori parameter guesses

    :return (popt, pcov): parameters values and covariances in tuple
    '''
    # fit function
    popt, pcov = curve_fit(function, x, y, p0, maxfev = 500000)
    return (popt, pcov)


def fit_func_fixed(function, x, y, p0):
    '''

    Function that fixes the value of mu for respective fits, and separates 
    the ugly parameters of curve_fit from the main codeline to reduce clutter.

    :param function: Input function
    :param x: X values
    :param y: Y values
    :param p0: A priori parameter guesses

    :return (popt, pcov): parameters values and covariances in tuple

    '''

    popt, pcov = curve_fit(lambda x, A1, mu1, sigma1, A2, sigma2: function(x, A1, mu1, sigma1, A2, (mu1*2), sigma2), x, y, p0, maxfev = 500000)
    return (popt, pcov)


def produce_data_points(file_path, bin_no = 100, plot = False):
    '''
    Produce data points (x,y) for fitting, and plot if asked for.
    Also includes cleaning the data (removing below zero values, setting values smaller than 1 = 0)
    
    :param file_path: Source file you're producing from
    :param bin_no: Number of bins in histogram
    :param plot: Plotting toggle

    :return (x,y): tuple of x and y values
    '''

    data = np.load(file_path)

    # scrape data < 0 away
    data = data[(data > 0)]

    heights, bin_pos = np.histogram(data, bins = bin_no)

    bin_pos = bin_pos[:-1] + np.diff(bin_pos)/2


    # plotting
    if (plot == True):
        plt.plot(bin_pos, heights)
        plt.title("Data points")
        plt.yscale('log')
        plt.show()


    return (bin_pos, heights)


def find_PV(x, y, prom = 100, plot = False):
    '''
    Peak and Valley finding code

    :param x: X values
    :param y: Y values
    :param prom: Prominence of peaks and valleys
    :param plot: Plotting toggle

    :return (peaks, valleys): index for x at which y peaks and valleys
    '''
    # hacky way to search for peaks by toggling prominence slightly until finding what you want.
    # warning! really hacky, so you can just get trapped
    while True:
        
        # find valley
        valleys, _ = find_peaks(-y, prominence = prom)

        # find peaks
        peaks, _ = find_peaks(y, prominence = prom)

        # number of peaks is now 1 not 2
        if ((len(peaks) == 1) and (len(valleys) == 1)):
            break
        # not enough peaks, decrease prominence
        elif ((len(peaks) < 2) and (len(valleys) == 0)):
            # increase prominence incrementally. This will almost 100% break
            prom = 0.9*prom
        else:
            prom = 1.1*prom
        
    print("Peak(s) found at: ({}, {})".format(x[peaks], y[peaks]))
    print("Valleys(s) found at: ({}, {})".format(x[valleys], y[valleys]))

        # plotting
    if (plot == True):
        plt.plot(x, y)
        plt.title('Peaks and valleys')
        plt.plot(x[peaks], y[peaks], 'xr', label=r'Peaks')
        plt.plot(x[valleys], y[valleys], 'xb', label=r'Valleys')
        plt.yscale('log')
        plt.legend()
        plt.show()

    return (peaks, valleys)

def create_fit(fnc, x, y, p, v, n_gauss, a_prio, mu = 0):
    '''
    General fitting function for N gaussians including limiting function

    :param fnc:     Input function
    :param x:       X values
    :param y:       Y values
    :param p:       Peak index
    :param v:       Valley index
    :param n_gauss: Number of gaussian fits
    :param a_prio:  A priori guesses for fit
    :param mu:      Mean (decides whether fixed or not)
    '''
    # distance between peak and valley
    dif = (p - v) + (1*n_gauss) # don't lose the tail!


    # trim data around gaussians, based on number
    x = x[v : (p + dif*n_gauss)]
    y = y[v : (p + dif*n_gauss)]

    # fix mu values false, most open fitting. WORKS BEST FOR 1 GAUSSIAN
    if (mu == 0):
        popt, pcov = fit_func(fnc, x, y, p0 = a_prio)
    else:
        popt, pcov = fit_func_fixed(fnc, x, y, p0 = a_prio)
    

    return (popt, pcov)


    
def plot_fit(fnc, x, y, x_trim, y_trim, popt, mu = 0, n_ga = 1):
    '''
    Plots the fitting functions, as well as the composite gaussians.

    :param fnc:     Input function
    :param x:       X values
    :param y:       Y values
    :param x_trim:  Trimmed x values
    :param y_trim:  Trimmed y values
    :param popt:    Fill parameters
    :param mu:      Fixed mean (Remove this functionality)
    :param n_ga:    number of gaussian peaks for fitting
    '''

    # create the linear x space for clean plotting
    x_space = np.linspace(min(x), max(x), len(x))
    
    # Plot data
    plt.plot(x, y, label = r'Data')

    # if non-fixed mu
    if (mu == 0):
        y_space = fnc(x_space, *popt)
        y_space[y_space < 1] = 1                                                        # squashing values near 0 for plotting purposes
        plt.plot(x_space, y_space, linewidth=2.5, label = r'Fitted function')
    else:
        y_space = fnc(x_space, popt[0], popt[1], popt[2], popt[3], mu*2, popt[4])
        y_space[y_space < 1] = 1                                                        # squashing values near 0 for plotting purposes
        plt.axvline(x_trim[0], color='blue', linestyle = 'dotted', linewidth = 1)                               # plotting limits of fitting
        plt.axvline(x_trim[-1], color='blue', label = r'Fitting limits', linestyle = 'dotted', linewidth = 1)
        plt.plot(x_space, y_space, label = r'Fitted function', linewidth = 2.5)

        y_space_n = []
        for i in range(0,n_ga):
            name = "Gaussian " + str(i)
            # plot individual gaussian contribution
            y_space_n.append(gauss_1(x_space, popt[(i*3)], (popt[1])*(i+1), popt[(i+1)*2])) # goofy looping lmao
            # trim y space lengths
            y_space_n[i][y_space_n[i] < 1] = 1
            plt.plot(x_space, y_space_n[i], label = name, linestyle = '-')
        

        

    plt.title('Fit')
    plt.legend()
    plt.yscale('log')
    plt.show()
    
def print_parameters(popt,pcov,labels):
    '''
    Prints fitting parameters

    :param popt:        Fit parameters
    :param pcov:        Fit covariances
    :param labels:      Labels of parameters
    '''
    print('===============================')
    print("        Fitting output      ")
    print('===============================')
    for i in range(len(popt)):
        print("{}: {:.4f} \u00B1 {:.4f}".format(labels[i], popt[i], np.sqrt(pcov[i][i]) )) # taking diagonal covariances as errors
    print('===============================')
    return 0


def trim_data(x, y, p, v, n_gauss, plot = False):
    '''
    Trim data to apply fit over

    :param x:       X Values
    :param y:       Y Values
    :param p:       Peak Values
    :param v:       Valley Values
    :param n_gauss: Number of Gaussians
    :param plot:    Plotting toggle

    :return (xtrim, ytrim): Trimmed x and y values
    '''

    # distance between peak and valley
    dif = (p - v) + (1*n_gauss) # don't lose the tail!

    lower = v
    upper = (p + dif*(n_gauss+1)) # +1 arbitrary

    # trim data around gaussians, based on number
    x_trim = x[lower : upper]
    y_trim = y[lower : upper]

    if (plot == True):
        plt.plot(x_trim, y_trim)
        plt.title('Fitting Range')
        plt.yscale('log')
        plt.show()

    return (x_trim, y_trim)

def main():

    '''
    Collects charge histogram npy file and outputs N gaussian fits to it.
    '''

    file_path = "output/RUN_38/ADC_data.npy"    # file path
    prom = 100                                  # prominence of peaks
    n_gauss = 2                                 # number of gaussians
    fnc = [gauss_1, gauss_2]                    # the separate functions
    verbose = False                             # verbose 
    fix = True                                  # fixed first mu value for gaussian

    n_g = n_gauss-1                             # custom n variable because its useful           

    ################################## INITIAL PROCESSING ##################################

    # produce data
    x, y  = produce_data_points(file_path, bin_no = 150, plot = verbose)
    
    # make them numpy arrays for easier manipulation
    x = np.array(x)
    y = np.array(y)

    # find peaks
    p, v = find_PV(x, y, plot = verbose)
    
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

            a_prio.append([370, 0.02])                          # one gaussian
            a_prio.append([2986, x[p], 0.02, 500, 0.01])               # two gaussians

            print("mu fixed to: {}".format(x[p]))


            mu_fix = x[p]
        else:
            print("Functionality for fixed mu only available for 2 gaussian model!")
            exit()

    ################################## DATA PROCESSING, FIT PRODUCTION ##################################


    x_trim, y_trim = trim_data(x, y, p, v, n_gauss, plot = verbose)




    popt, pcov = create_fit(fnc[n_g], x_trim, y_trim, p, v, n_gauss, a_prio[n_g], mu = mu_fix)

    #print popt wrt labels
    print_parameters(popt,pcov, labels[n_g])

    # plot fit
    plot_fit(fnc[n_g], x, y, x_trim, y_trim, popt, mu = mu_fix, n_ga = n_gauss)

    

        



if __name__ == "__main__":

    main()
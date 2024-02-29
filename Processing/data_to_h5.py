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


import sys,os,os.path

# tell python to look for the path to analyse
module_path = os.path.abspath(os.path.join('..'))
if module_path not in sys.path:
    sys.path.append(module_path)

from Analyse.core import processing as proc

def main(file_path):
    proc.read_raw_h5(file_path, verbose = False, save_h5=True, print_mod = 10000)


if __name__ == "__main__":
    # this is it taking the file directory from the first argument,
    # which is generally passed through in process_analyse_locally_auto.sh
    print("Processing file at:\n{}".format(sys.argv[1]))
    main(sys.argv[1])
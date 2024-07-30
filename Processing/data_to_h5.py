'''
    John Waiton - 2024
        This script is the basis for which files will be converted to h5 files
        rather than the standard ROOT files

        At the moment it currently has all the working for this system, as it's
        currently unfinished

'''

import numpy as np
import argparse

import sys,os,os.path

# tell python to look for the path to analyse
module_path = os.path.abspath(os.path.join('..'))
if module_path not in sys.path:
    sys.path.append(module_path)

from Analyse.core import processing as proc



'''
argument list:
1 - file path
2 - save_h5
3 - cook flag
4 - verbose flag
5 - printing modifier
'''
parser = argparse.ArgumentParser(description='Process wavedump .dat data.')

parser.add_argument('filepath')
parser.add_argument('-t', '--type', choices = ['bin', 'txt'], default = 'bin', help = 'file type is either binary (.bin) or asci (.txt), select one')
parser.add_argument('-h5', action='store_true')
parser.add_argument('-c', '--cook', action='store_true')
parser.add_argument('-v', '--verbose', action='store_true')
parser.add_argument('-p', '--print')

args = parser.parse_args()
    

def main(arguments):

    if args.type == 'bin':
        proc.read_raw_h5(args.filepath, args.h5, args.cook, args.verbose, args.print)
    if args.type == 'txt':
        proc.read_ascii_h5(args.filepath, args.h5, args.cook, args.verbose, args.print)



if __name__ == "__main__":
    # this is it taking the file directory from the first argument,
    # which is generally passed through in process_analyse_locally_auto.sh
    print("Processing file at:\n{}".format(sys.argv[1]))
    main(sys.argv)
# Program that will automatically run the bash script 'source XY_move_auto.sh' within each RUN0000NN file instance
# Currently set up to run within John/XYTesting/RUN0000NN

from contextlib import contextmanager
import os
import subprocess
import sys


# Create context manager to control moving to each RUN directory

@contextmanager
def cd(newdir):
    # Collect CWdir, then move to new dir
    prevdir = os.getcwd()
    os.chdir(os.path.expanduser(newdir))
    # If possible to move to this directory, hand back to 'with' statement that calls generator
    # At end, return to previous Wdir
    try:
        yield
    finally:
        os.chdir(prevdir)

def analyse_run(run_no, base_dir):
    # Is given the Data_Storage directory already.
    # Set specific directory in here. For now this is /John/XYTesting/RUN0000NN
    # Will then move to this directory and run source xy_move_analyse.sh to create text files

    # Convert to list and then back to string to expose hidden characters
    base_dir = str([base_dir])
    # Remove hidden characters to give correct directory format
    base_dir = base_dir[2:]
    base_dir = base_dir[:-4]

    # Create string format for directory movement
    string_base = "John/XYTesting/"

    # Create proper run formatting: RUN0000NN
    run = "RUN"
    run += str(run_no).zfill(6)
    run += "/"
    # Full string
    file_path = string_base + run
    file_path = base_dir + file_path
    print(file_path)
    # move to RUN directory and run source XY_move_analyse.sh
    with cd(file_path):
        # Checking that we're in the correct directory before analysis
        print(os.getcwd())
        # Run analysis code
        # NOTE, If XY
        subprocess.call(["source XY_move_analyse.sh"], shell=True)


def automate_analyse(start_run, end_run):
    # Creates the loop over which the data is analysed
    # Input the starting run, and the ending run, will loop over and allow analysis
    i = int(start_run)

    # Create string format for directory movement

    # Hacky method to find the true directory from the $WM_DAQ alias.
    # Not clean, may break later, can be hard-coded in, but for the time being this should work across multiple machines without issue

    # Run shell code that will print alias, and collect it as output, universal_newlines removes issue with encoding
    processes = subprocess.Popen("echo $WM_DATA", shell=True, stdout=subprocess.PIPE, universal_newlines=True).communicate()[0]
    print(processes)


    # +1 to ensure that the end_run is included
    while (i < int(end_run)+1):
        analyse_run(i, processes)
        i+=1

#automate_analyse(24, 28)


# Collect arguments to run, if you dont have enough, stop the code
# This needs to be adjusted to allow for starting run numbers
if len(sys.argv) == 3:
    # ignoring the name of the python script, so choosing second argument
    automate_analyse(sys.argv[1], sys.argv[2])
else:
    print("collect_data takes exactly 2 arguments (" + str(len(sys.argv)-1) + ") given")
    print("Please input starting run, and ending run")

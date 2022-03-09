import matplotlib.pyplot as plt
import numpy as np
import subprocess
import time
import sys
import os

## Program that automates the data collection and cooking for the XY table
## Requires two inputs to run, or can be called separately to make use of the functions within
##

# Function that reads in the XY positions into a list and returns said list
def read_positions(filename):
	'''
	Reads the positions file out into a list of strings to be used for automation.
	
	:param filename:	Name of .txt file
	:return contents:	List of positions as strings
	'''
	# Open file
	with open(filename) as f:
		contents = f.readlines()

	# Check that file has been read correctly (length)
	if (len(contents) > 1):
		print("File read of length: " + str(len(contents)))
	else:
		print("File length too short to process (please include more positions!)")

	return contents
	
# Writing basic script that utilises the moveabsxy function from scripts.sh
def move_table(positionx, positiony):
	'''
	Moves the XY table using positions passed through as strings
	Only works if table is motors are on (table_on())
	
	:param positionx: Absolute X position for table
	:param positiony: Absolute Y position for table
	'''
	# Apply as strings
	x = positionx
	y = positiony
	print("Moving to " + x + " " + y)
	# Running shell commands
	rc = subprocess.call(["echo -ne 'X:MOV:ABS " + x + "\n' > /dev/ttyUSB0"], shell=True)
	rc = subprocess.call(["echo -ne 'Y:MOV:ABS " + y + "\n' > /dev/ttyUSB0"], shell=True)

def table_off():
	'''
	Turns the XY-table motors off.
	'''
	print("Motors disabled")
	rc = subprocess.call(["echo -ne 'X:DISABLE\n' > /dev/ttyUSB0"], shell=True)
	rc = subprocess.call(["echo -ne 'Y:DISABLE\n' > /dev/ttyUSB0"], shell=True)

def table_on():
	'''
	Turns the XY-table motors on.
	'''
	print("Motors enabled")
	rc = subprocess.call(["echo -ne 'X:ENABLE\n' > /dev/ttyUSB0"], shell=True)
	rc = subprocess.call(["echo -ne 'Y:ENABLE\n' > /dev/ttyUSB0"], shell=True)


def ping_position():
	'''
	Pings the current position of the XY-table carriage to the designated cat terminal
	For more information on the cat terminal, please look at /XY-Scripts/
	'''
	# will ping current table position
	rc = subprocess.call(["echo -ne 'X:ABS?\n' > /dev/ttyUSB0"], shell=True)
	rc = subprocess.call(["echo -ne 'Y:ABS?\n' > /dev/ttyUSB0"], shell=True)
	print("Coordinates sent to cat terminal.")

def collect_data(collect_bash, process_bash, run_no):
	'''
	Given two bash scripts to collect and process the data, and the run number.
	This code will use wavedump to collect the data, then use /data_analysis/ to move and cook the data.
	
	:param collect_bash:	Data collection bash script
	:param process_bash:	Data Processing bash script
	:param run_no:		The current run number
	'''
	# Collect data
	rc = subprocess.call([collect_bash], shell=True)

	p = subprocess.run([process_bash], input = (run_no + "\n").encode(), stdout=subprocess.PIPE)
	
	return True
	

def automate(start_run, filename):
	'''
	The main sequence for code automation
	Collects postional data from the input file.
	Then runs through a loop for these positions.
	For each position it moves the XY carriage, disables it, collects and processes data and then reenables the carriage and moves again
	Does this until all positions have been reached.
	
	:param start_run:	Number of the starting run (e.g. 000236)
	:param filename:	Filename of the positions textfile
	'''

	# Collect position information
	positional = read_positions(filename)
	# Create loop based on length of positional data
	for i in range(len(positional)):
		# Split position string into x,y
		spl_pos = str.split(positional[i])
		move_table(spl_pos[0], spl_pos[1])
		# Sleep for 20 seconds, otherwise table movement wont be able to complete before data is processed
		print("Moving table...")
		time.sleep(20)

		# Ping position
		ping_position()

		# Turn off table
		table_off()	

		# Increment through runs. Need to do string->int->string conversion sadly
		# zfill fills the left side with zeroes, like is expected for RUN titles
		current_string = str(int(start_run)+i).zfill(6)
		print("Processing data for RUN" + current_string)
		# Collect data at position
		collect_data('./Run_wavedump_PCI_1_min_10_CH_NS_150.sh', './move_10_ch_nom_trig_auto.sh', current_string)
		# Turn table back on after data is collected, added print to check that this isnt done before data collection is finished
		print("Data processed, moving to next coordinates...")
		table_on() 
	
	return True



# Collect arguments to run, if you dont have enough, stop the code
# This needs to be adjusted to allow for starting run number, positions text file
if len(sys.argv) == 3:
    # ignoring the name of the python script, so choosing second argument
    automate(sys.argv[1], sys.argv[2])
else:
    print("collect_data takes exactly 2 arguments (" + str(len(sys.argv)-1) + ") given")
    print("Please input starting run, and the positions file.")



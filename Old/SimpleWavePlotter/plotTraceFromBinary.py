# A very simple program to plot wavedump traces
#
# Author   Matt Needam
#
# Modified 
# gary.smith@ed.ac.uk
# 17 10 18
# changed program name and added comments
# commented out code in progress
#
# How to run:
# $ python plotTraceFromBinary.py
#
# Requirements:
# data must be stored in ../../Data
#
# Notes:
# commented code is a work in progress to plot all events

# 2D plotting library
import matplotlib.pyplot as plt
# fundamental scientic package including
# n-dimensional array
import numpy as np

fileName = "../../Data/wave_0.dat" 

f = open(fileName,"r")

# Read in all events from binary data
b = np.fromfile(f,dtype=np.float32)

# determine number of events
# (each event has 1024 bins)
nevent = len(b)/1024

# split into events
x = np.split(b,nevent)

# ?
np.set_printoptions(suppress=False, threshold=10000)

# print values to screen
#print(x[0])

# 
#time = np.linspace(0,1024,1024)

#for ievent in range(1000):
#	plt.plot(time,x[ievent])
#	name = "event_"+str(ievent) + ".png"
#	plt.savefig(name)

# plot and display the first event
plt.plot(x[0])
plt.show()

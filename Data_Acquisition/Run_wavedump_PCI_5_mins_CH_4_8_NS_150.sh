#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && sleep 300s && echo "s" && sleep 3s && echo "q") | wavedump /etc/wavedump/WaveDumpConfig_PCI_Ch_4_5_6_7_8_NS_150.txt

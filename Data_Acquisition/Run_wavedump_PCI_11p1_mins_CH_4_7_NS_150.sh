#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && sleep 667s && echo "s" && sleep 3s && echo "q") | wavedump /etc/wavedump/WaveDumpConfig_PCI_Ch_4_5_6_7_NS_150.txt

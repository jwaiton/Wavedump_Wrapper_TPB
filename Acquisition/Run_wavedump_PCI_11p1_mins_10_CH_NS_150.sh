#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && sleep 667s && echo "s" && sleep 3s && echo "q") | wavedump /etc/wavedump/WaveDumpConfig_PCI_CH_10_NS_150.txt

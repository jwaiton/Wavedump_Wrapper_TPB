#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && echo "P" && sleep 120s && echo "s" && sleep 3s && echo "q") | wavedump /etc/wavedump/WaveDumpConfig_PCI_NS_150.txt

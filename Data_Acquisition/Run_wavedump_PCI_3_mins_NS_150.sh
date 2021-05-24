#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && sleep 180s && echo "s" && sleep 3s && echo "q") | wavedump /etc/wavedump/WaveDumpConfig_PCI_NS_150.txt

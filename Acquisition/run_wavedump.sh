#!/bin/bash

(sleep 3s && echo "W" && sleep 3s && echo "s" && sleep 600s && echo "s" && sleep 3s && echo "q") | wavedump ./WaveDumpConfig.txt

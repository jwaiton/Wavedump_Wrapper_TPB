#!/bin/bash

(sleep 3s && echo "s" && sleep 3s && echo "W" && sleep 300s && echo "s" && sleep 3s && echo "q") | wavedump_G_SPE_DR

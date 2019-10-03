#!/bin/bash

(sleep 3s && echo "s" && sleep 3s && echo "W" && sleep 1000s && echo "s" && sleep 3s && echo "q") | wavedump

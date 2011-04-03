#!/bin/bash
scons run
gnuplot plot.gp
eog plot.png

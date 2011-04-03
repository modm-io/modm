set terminal png
set output "plot.png"
set grid
set title 'S-Curve Controller'
set lmargin 6
# set size ratio 0.5	# y axis is two times longer
# set y2tics

# Done interactively, this takes gnuplot into multiplot mode
# and brings up a new prompt ("multiplot >" instead of "gnuplot >")
set multiplot

# plot the first graph so that it takes the first half of the screen
set size 1,0.6
set origin 0,0.4

plot "plot.dat" using 1:2 axes x1y1 title "Speed" with lines, \
	"plot.dat" using 1:3 axes x1y2 title "Position" with lines

# plot the second graph
set title 
set size 1,0.4
set origin 0,0

plot "plot.dat" using 1:4 title "Acceleration" with lines
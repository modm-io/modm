set terminal png
set output "plot.png"
set grid
set title 'S-Curve Controller'
set lmargin 6

set linestyle 1 lt 1 lw 2
set linestyle 2 lt 2 lw 2
set linestyle 3 lt 3 lw 2 ps 7

# set size ratio 0.5	# y axis is two times longer
# set y2tics

# Done interactively, this takes gnuplot into multiplot mode
# and brings up a new prompt ("multiplot >" instead of "gnuplot >")
set multiplot

# plot the first graph so that it takes the first half of the screen
set size 1,0.6
set origin 0,0.4
set yrange  [-0.5:8.5]
set y2range [-0.5:8.5]

plot "plot.dat" using 1:2 axes x1y1 title "Speed"    with steps ls 1, \
	 "plot.dat" using 1:3 axes x1y2 title "Position" with steps ls 2

# plot the second graph
set title 
set size 1,0.4
set origin 0,0
set yrange [-1.2:1.2]

plot "plot.dat" using 1:4 title "Acceleration" with steps ls 3
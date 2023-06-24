#Gnuplot File for plotting Data.txt

set autoscale
unset logscale
unset log
unset label
unset xlabel
unset ylabel
unset title
set xtic auto
set ytic auto
set title "Energy of System"
set xlabel "# of Iterations"
#set ylabel "Energy"
set noytic
set multiplot
set nokey
plot "Data.txt" using 1:4 title 'Delta Energy' with lines 2
set xlabel "# of Iterations"
set nokey
plot "Data.txt" using 1:5 title 'Temperature' with lines
#plot "Data.txt" using 1:2 title 'Energy' with lines , \
#     "Data.txt" using 1:3 title 'Min Energy' with lines
set nomultiplot

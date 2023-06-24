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
set key 100,.85
set title "Magnetization for B=.06" font "Times-Roman,40"
set xlabel "# of Iterations" font "Times-Roman,30"
set ylabel "Magnetization projected in B-field dir" font "Times-Roman,30"
plot "formateddata6_1.txt" title 'T=.01' with lines, \
     "formateddata6_6.txt" title 'T=.06' with lines, \
     "formateddata6_11.txt" title 'T=.11' with lines

#plot "Data.txt" using 1:2 title 'Energy' with lines , \
#     "Data.txt" using 1:3 title 'Min Energy' with lines
#set nomultiplot

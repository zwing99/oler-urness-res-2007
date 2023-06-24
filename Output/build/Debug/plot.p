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
set key 100,.75
set title "Magnetization for T=0" font "Times-Roman,40"
set xlabel "# of Iterations" font "Times-Roman,30"
set ylabel "Magnetization projected in B-field dir" font "Times-Roman,30"
plot "formateddata5_0.txt" title 'B=.05' with lines, \
     "formateddata6_0.txt" title 'B=.06' with lines, \
     "formateddata7_0.txt" title 'B=.07' with lines, \
     "formateddata8_0.txt" title 'B=.08' with lines, \
     "formateddata9_0.txt" title 'B=.10' with lines

#plot "Data.txt" using 1:2 title 'Energy' with lines , \
#     "Data.txt" using 1:3 title 'Min Energy' with lines
#set nomultiplot

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
set key 400,.060035
f1(x) = a*x + b
fit f1(x) 'mydata5_6.txt' using 1:6 via a,b
#set title "Temperature for B=.05 and T=" font "Times-Roman,40"
set xlabel "# of Iterations" font "Times-Roman,30"
set ylabel "Temperature" font "Times-Roman,30"
plot "mydata5_6.txt" using 1:6 title 'Real Temperature', \
     "mydata5_6.txt" using 1:5 with lines title 'Given Temperature', \
     f1(x) title 'Linear Fit for Real Temperature'
#plot "Data.txt" using 1:2 title 'Energy' with lines , \
#     "Data.txt" using 1:3 title 'Min Energy' with lines
#set nomultiplot

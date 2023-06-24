f(x) = a*tanh(b/x)
a = 1.0
b = 1.0
fit f(x) "mydata_mod2.txt"  via a, b
set autoscale
unset logscale
unset log
unset label
unset xlabel
unset ylabel
unset title
set xtic auto
set ytic auto
set key .25,.85
set title "Saturation Magnetization for B=.03" font "Times-Roman,40"
set xlabel "Temperature (T)" font "Times-Roman,30"
set ylabel "Magnetization (M)" font "Times-Roman,30"
plot "mydata_mod2.txt" title 'Magnetization Saturation for B=.03', \
     f(x) title 'f(x) = a tanh( b/T )'
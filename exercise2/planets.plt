#!/usr/bin/env gnuplot

set term pdf;
set termopt enhanced

set output "planets.pdf"
set multiplot layout 2,1

set title "Planets doing circles and stuff"

#set xlabel 'Temperatur {/Helvetica-Italic T} / K';
#set ylabel "Luminosität {/Helvetica-Italic L} / {/Helvetica-Italic L_☉}"
set xrange [-8 : 8];
set yrange [-8 : 8];
#set xtics (3000, 4500, 7000, 10000, 15000, 20000, 30000, 40000);
#set format x "%.0f"
#set format y "10^{%T}"
plot "planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using 1:2 title "Sun" with points,\
"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using 4:5 title "Earth" with lines,\
"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using 7:8 title "Jupiter" with lines;
#plot "planets.bin" binary format="%double%double" title "Earth" with lines;
set title "Angular Momentum"
unset xrange
unset yrange
#set yrange [0 : -15000]
#set logscale y
first(x) = ($0 > 0 ? base : base = x)
plot "planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using (first($3+$6+$9) / ($3+$6+$9)) title "%L_z" with lines;#,\
#"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using ($6) title "L_z Earth" with lines,\
#"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using ($9) title "L_z Jupiter" with lines;
#unset multiplot

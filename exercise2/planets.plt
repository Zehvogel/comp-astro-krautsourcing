#!/usr/bin/env gnuplot

set term pdf size 5,5;
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
s=1
plot "planets.bin" binary format="%float%float%float%float%float%float%float%float%float%float" using 2:3 every s title "Sun" with points,\
"planets.bin" binary format="%float%float%float%float%float%float%float%float%float%float" using 5:6 every s title "Earth" with lines,\
"planets.bin" binary format="%float%float%float%float%float%float%float%float%float%float" using 8:9 every s title "Jupiter" with lines;
#plot "planets.bin" binary format="%double%double" title "Earth" with lines;
set title "Angular Momentum and Energy"
unset xrange
unset yrange
set xlabel "Time [days]"
set ylabel "Momentum"
set y2label "Energy"
set y2tics
#set yrange [0 : -15000]
#set logscale y
first(x) = ($0 > 0 ? base : base = x)
plot "planets.bin" binary format="%float%float%float%float%float%float%float%float%float%float" using (first($4+$7+$10) / ($4+$7+$10)) every s title "%L_z" with lines axes x1y1,\
"planets.bin" binary format="%float%float%float%float%float%float%float%float%float%float" using (first($1) / $1) every s title "%E" with lines axes x1y2;
#"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using ($6) title "L_z Earth" with lines,\
#"planets.bin" binary format="%float%float%float%float%float%float%float%float%float" using ($9) title "L_z Jupiter" with lines;
#unset multiplot

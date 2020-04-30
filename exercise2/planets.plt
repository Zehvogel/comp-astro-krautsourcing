#!/usr/bin/env gnuplot

set term pdf;
set termopt enhanced

set title "Planets doing circles and stuff"

#set xlabel 'Temperatur {/Helvetica-Italic T} / K';
#set ylabel "Luminosität {/Helvetica-Italic L} / {/Helvetica-Italic L_☉}"
set output "planets.pdf"
set xrange [-3 : 3];
set yrange [-3 : 3];
#set xtics (3000, 4500, 7000, 10000, 15000, 20000, 30000, 40000);
#set format x "%.0f"
#set format y "10^{%T}"
plot "planets.bin" binary format="%float%float%float%float" using 1:2 title "Sun" with points,\
"planets.bin" binary format="%float%float%float%float" using 3:4 title "Earth" with lines;
#plot "planets.bin" binary format="%double%double" title "Earth" with lines;

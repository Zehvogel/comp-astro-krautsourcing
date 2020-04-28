#!/usr/bin/env gnuplot

set term pdf;
set termopt enhanced

set title "Planets doing circles and stuff"

#set xlabel 'Temperatur {/Helvetica-Italic T} / K';
#set ylabel "Luminosität {/Helvetica-Italic L} / {/Helvetica-Italic L_☉}"
set output "planets.pdf"
#set xrange [35000 : 2500] reverse;
#set xtics (3000, 4500, 7000, 10000, 15000, 20000, 30000, 40000);
#set format x "%.0f"
#set format y "10^{%T}"
plot "planets.bin" binary format="%*float32%*float32" title "Earth" with points;

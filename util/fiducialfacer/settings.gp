# # Line style for axes
# set termoption dashed

set style line 80 lt 1
set style line 80 lt rgb "#404040" lw 2

# Line style for grid
set style line 81 lt 3  # dashed
set style line 81 lt rgb "#606060" lw 1 # grey

# set style line 82 lt 1  # dashed
# #set style line 82 lt rgb "#D4AA2C" lw 2
# set style line 82 lt rgb "#F1C232" lw 3
# set style line 83 lt 1  # dashed
# set style line 83 lt rgb "#E69138"lw 3

# set style line 90 lt 1
# set style line 90 lt rgb "#404040" lw 1
# set style arrow 90 nohead ls 90


# dark red :    #990000
# dark blue :   #0B5394
# dark green :  #38761D
# dark yellow : #967100

# QGIS screenshot SWISS TP color #1155CC
# QGIS screenshot TRACK FP color #CC0000
# QGIS screenshot SWISS FN color #F1C232

# set style line 1 lt 1
# set style line 2 lt 1
# set style line 3 lt 1
# set style line 1 lt rgb "#990000" lw 6 pt 7 ps 0.8
# set style line 2 lt rgb "#0B5394" lw 6 pt 7 ps 0.8
# set style line 3 lt rgb "#38761D" lw 6 pt 7 ps 0.8

set title font "Sans,18"
set key font "Sans,20"
set xtics font "Sans,9"
set ytics font "Sans,9"

#set grid back linestyle 81
set border back linestyle 80
#unset key
set key at 14.7,38

set lmargin at screen 0.16;
set rmargin at screen 0.93;
#set bmargin at screen 0.1;
set tmargin at screen 0.98;

set xlabel offset 0,0.5,0
set ylabel offset 2.8,0,0

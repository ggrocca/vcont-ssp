# gnuplot -e "../path/to/data/" gnuplot.gp


pwd=GPVAL_PWD
cd dir

set terminal pdfcairo size 10cm,20cm

### life and strength

#set term aqua 0
set output "query-terrain-strength.pdf"
set multiplot layout 3,1
 set yrange[0:1200]
 set xlabel "spots"
 set ylabel "strength"
  plot "stats-sort-terrain-strength.dat" \
       title "terrain spots, all"
  plot "stats-sort-terrain-truepositive-strength.dat" \
       title "terrain spots, true positive"
  plot "stats-sort-terrain-falsepositive-strength.dat" \
       title "terrain spots, false positive"
unset multiplot

#set term aqua 1
set output "query-terrain-life.pdf"
set multiplot layout 3,1
 set yrange[0:15]
 set xlabel "spots"
 set ylabel "life"
  plot "stats-sort-terrain-life.dat" \
       title "terrain spots, all"
  plot "stats-sort-terrain-truepositive-life.dat" \
       title "terrain spots, true positive"
  plot "stats-sort-terrain-falsepositive-life.dat" \
       title "terrain spots, false positive"
unset multiplot

#### life+strength
set output "query-terrain-everything.pdf"
set multiplot layout 3,1
 set yrange[0:15]
 set xrange[0:1200]
 set xlabel "strength"
 set ylabel "life"
  plot "stats-sort-terrain-lifestrength.dat" using 2:1 \
       title "terrain life vs strength, all" 
  plot "stats-sort-terrain-lifestrength.dat" using 2:($3 > 0 ? $1 : 1/0) \
       title "terrain life vs strength, true positive"
  plot "stats-sort-terrain-lifestrength.dat" using 2:($4 > 0 ? $1 : 1/0) \
       title "terrain life vs strength, false positive"
unset multiplot

unset xlabel
unset ylabel
unset xrange
unset yrange



load pwd.'YlGnBu.plt'

set terminal pdfcairo size 20cm,20cm
set colorbox vert user origin screen .88,.66 size .025,.2


##### ROC
set xlabel "fpr"
set ylabel "tpr"

set output "terrain-ROC.pdf"
set key autotitle columnheader
  plot [0:1][0:1] "stats-roc-terrain.dat" using "fpr":"tpr" title "ROC", \
       x lt 3 lc 3

set style fill transparent solid 0.8 noborder
set output "terrain-ROC-life.pdf"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k3 stats-roc-terrain.dat' using "fpr":"tpr":"life" \
       title "ROC" w circles lc palette
set style fill empty

set style fill transparent solid 0.8 noborder
set output "terrain-ROC-strength.pdf"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k4 stats-roc-terrain.dat' using "fpr":"tpr":"strength" \
       title "ROC" w circles lc palette
set style fill empty


##### PR
set xlabel "tpr"
set ylabel "ppv"

set output "terrain-PR.pdf"
set key autotitle columnheader
  plot [0:1][0:1] "stats-roc-terrain.dat" using "tpr":"ppv" title "PR", \
        1-x lt 3 lc 3

set style fill transparent solid 0.8 noborder
set output "terrain-PR-life.pdf"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k3 stats-roc-terrain.dat' using "tpr":"ppv":"life" \
       title "PR" w circles lc palette
set style fill empty

set style fill transparent solid 0.8 noborder
set output "terrain-PR-strength.pdf"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k4 stats-roc-terrain.dat' using "tpr":"ppv":"strength" \
       title "PR" w circles lc palette
set style fill empty

  
##### heat maps of indices
set xlabel "life"
set ylabel "strength"
set xrange[0:15]
set yrange[0:1200]
  
#load pwd.'/gnuplot-colorbrewer/diverging/RdYlBu.plt'
set palette negative
#set cbrange[-1:1]

set output "terrain-markedness.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"markedness" \
     title "markedness"

set output "terrain-informedness.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"informedness" \
     title "informedness"

set output "terrain-mcc.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"mcc" \
     title "mcc"

#load pwd.'/gnuplot-colorbrewer/sequential/YlGnBu.plt'
#set palette negative
#set cbrange[0:1]

set output "terrain-f1.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"f1" \
     title "f1"

set output "terrain-f2.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"f2" \
     title "f2"

set output "terrain-f05.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"f05" \
     title "f05"

set output "terrain-ppv.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"ppv" \
     title "ppv"

set output "terrain-tpr.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"tpr" \
     title "tpr"

set output "terrain-fpr.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"fpr" \
     title "fpr"

set output "terrain-fnr.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"fnr" \
     title "fnr"

set output "terrain-tnr.pdf"
set key autotitle columnheader
set pm3d map
splot "stats-roc-terrain.dat" \
     using "life":"strength":"tnr" \
     title "tnr"

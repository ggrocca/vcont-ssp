pwd=GPVAL_PWD
cd dir

set terminal pdfcairo size 20cm,20cm

#### indicators peak
set yrange[-1:1]

set output "terrain-markedness.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"markedness" \
     title "markedness"

set output "terrain-informedness.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"informedness" \
     title "informedness"

set output "terrain-mcc.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"mcc" \
     title "mcc"

# dups
# maxcandidates
set yrange[0:200]

set output "terrain-duplicates.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"duplicates" \
     title "duplicates"

set output "terrain-maxcandidates.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"maxcandidates" \
     title "maxcandidates"


# normal indices
set yrange[0:1]

set output "terrain-f1.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"f1" \
     title "f1"


set output "terrain-f2.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"f2" \
     title "f2"

set output "terrain-f05.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"f05" \
     title "f05"

set output "terrain-ppv.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"ppv" \
     title "ppv"

set output "terrain-tpr.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"tpr" \
     title "tpr"

set output "terrain-fpr.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"fpr" \
     title "fpr"

set output "terrain-fnr.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"fnr" \
     title "fnr"

set output "terrain-tnr.pdf"
set key autotitle columnheader
plot "stats-peaks-performance.dat" \
     using "life":"tnr" \
     title "tnr"


load pwd.'/YlGnBu-saturated.plt'

set terminal pdfcairo size 20cm,20cm
set colorbox vert user origin screen .88,.66 size .025,.2
set xtics 0.1
set ytics 0.1
set grid

##### PR-peaks
set xlabel "tpr"
set ylabel "ppv"

set output "terrain-peaks-PR.pdf"
set key autotitle columnheader
  plot [0:1][0:1] "stats-peaks-performance.dat" using "tpr":"ppv" title "PR-peak", \
        1-x lt 3 lc 3

set style fill transparent solid 1.0 noborder
set output "terrain-peaks-PR-life.pdf"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k3 stats-peaks-performance.dat' using \
       "tpr":"ppv":(0.005):"life" \
       title "PR-peak" w circles lc palette
set style fill empty




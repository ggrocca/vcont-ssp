# gnuplot -e "../path/to/data/" gnuplot.gp

cd dir

set terminal pdfcairo size 10cm,20cm

### life and strength

#set term aqua 0
set output "terrain-strength.pdf"
set multiplot layout 3,1
 set ylabel "strength"
 set xlabel "spots"
  plot "stats-sort-terrain-strength.dat" \
       title "terrain spots, all"
  plot "stats-sort-terrain-truepositive-strength.dat" \
       title "terrain spots, true positive"
  plot "stats-sort-terrain-falsepositive-strength.dat" \
       title "terrain spots, false positive"
unset multiplot

#set term aqua 1
set output "terrain-life.pdf"
set multiplot layout 3,1
  set ylabel "life"
  set xlabel "spots"
  plot "stats-sort-terrain-life.dat" \
       title "terrain spots, all"
  plot "stats-sort-terrain-truepositive-life.dat" \
       title "terrain spots, true positive"
  plot "stats-sort-terrain-falsepositive-life.dat" \
       title "terrain spots, false positive"
unset multiplot

#set term aqua 2
set output "curvature-strength.pdf"
set multiplot layout 3,1
 set ylabel "strength"
 set xlabel "spots"
  plot "stats-sort-curvature-strength.dat" \
       title "curvature spots, all"
  plot "stats-sort-curvature-truepositive-strength.dat" \
       title "curvature spots, true positive"
  plot "stats-sort-curvature-falsepositive-strength.dat" \
       title "curvature spots, false positive"
unset multiplot

#set term aqua 3
set output "curvature-life.pdf"
set multiplot layout 3,1
 set ylabel "life"
 set xlabel "spots"
  plot "stats-sort-curvature-life.dat" \
       title "curvature spots, all"
  plot "stats-sort-curvature-truepositive-life.dat" \
       title "curvature spots, true positive"
  plot "stats-sort-curvature-falsepositive-life.dat" \
       title "curvature spots, false positive"
unset multiplot

#### life+strength
set output "terrain-everything.pdf"
set multiplot layout 3,1
 set ylabel "life"
 set xlabel "strength"
  plot "stats-sort-terrain-lifestrength.dat" using 2:1 \
       title "terrain life vs strength, all" 
  plot "stats-sort-terrain-lifestrength.dat" using 2:($3 > 0 ? $1 : 1/0) \
       title "terrain life vs strength, true positive"
  plot "stats-sort-terrain-lifestrength.dat" using 2:($4 > 0 ? $1 : 1/0) \
       title "terrain life vs strength, false positive"
unset multiplot



function plot_scattered
{
    dir=$1
    input=$2
    output=$3
    x=$4
    y=$5
    title=$6

    echo $0 $FUNCNAME $@
    
    gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm
set xtics 0.1
set ytics 0.1
set grid

set xlabel '$x'
set ylabel '$y'

set output '$dir/$output'
set key autotitle columnheader
plot [0:1][0:1] '$dir/$input' using '$x':'$y' title '$title', 1-x lt 3 lc 3
_EOF_
}

function plot_scattered_color
{
    dir=$1
    input=$2
    output=$3
    x=$4
    y=$5
    z=$6
    sortkey=$7
    title=$8
    max=$9

    echo $0 $FUNCNAME $@
    
    gnuplot <<_EOF_
load 'YlGnBu-saturated.plt'

set terminal pdfcairo size 20cm,20cm
set colorbox vert user origin screen .88,.66 size .025,.2
set xtics 0.1
set ytics 0.1
set grid

set xlabel "$x"
set ylabel "$y"
 #set cbrange [*:*]
set cbrange [0:$max]

set style fill transparent solid 1.0 noborder
set output "$dir/$output"
set key autotitle columnheader
  plot [0:1][0:1] '<sort -g -k$sortkey $dir/$input' using \
       "$x":"$y":(0.005):"$z" \
       title "$title" w circles lc palette
_EOF_
}

# function plot_scattered_surface
# {
    
# }


function simple_plot
{
    dir=$1
    input=$2
    output=$3
    x=$4
    y=$5
    title=$6
    xmin=$7
    xmax=$8
    ymin=$9
    ymax=${10}
    
    echo $0 $FUNCNAME "$@"

    gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm
set xrange[$xmin:$xmax]
set yrange[$ymin:$ymax]
set output '$dir/$output'
set key autotitle columnheader
 #stats '$dir/$input' using "$x":"$y" nooutput 
 #max_y = GPVAL_DATA_Y_MAX
 #max_x = STATS_pos_max_y
 #set label 1 gprintf("Max = %g, Par=%g", max_y, max_x) at 2, min_y-0.2
plot '$dir/$input' \
     using "$x":"$y" \
     title "$title"
_EOF_
}


function heat_map
{
    dir=$1
    input=$2
    output=$3
    x=$4
    y=$5
    z=$6

    echo $0 $FUNCNAME $@
    
    gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm
load 'YlGnBu.plt'
set colorbox vert user origin screen .88,.66 size .025,.2
 #load 'gnuplot-colorbrewer/diverging/RdYlBu.plt'
 #unset xtics
 #unset ytics
set xtics auto
set ytics auto
 #unset grid

set cbrange [*:*]
 #set cbrange[-1:1]

set xlabel "$x"
set ylabel "$y"
 #set xrange[0:15]
 #set yrange[0:1200]
set xrange [*:*]
set yrange [*:*]
set palette negative

set output '$dir/$output'
set key autotitle columnheader
set pm3d map
splot '$dir/$input' \
     using "$x":"$y":"$z" \
     title "$z"
_EOF_
}

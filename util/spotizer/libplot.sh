
my_fontscale=1.7
my_dashlength=1.5

function plot_scattered
{
    dir=$1
    input=$2
    output=$3
    x=$4
    y=$5
    title=$6

    if [[ $x == "tpr" ]]; then
	xlabel="Recall"
    else
	xlabel=$x
    fi

    if [[ $y == "ppv" ]]; then
	ylabel="Precision"
    else
	ylabel=$y
    fi
    echo $0 $FUNCNAME $@

    if [[ $title == "Life+Drop" ]]; then
	linestyle=1
    elif [[ $title == "Drop" ]]; then
	linestyle=2
    elif [[ $title == "Jaara" ]]; then
	linestyle=3
    else
	linestyle=1
    fi

    
    gnuplot <<_EOF_
set terminal pdfcairo dl $my_dashlength size 20cm,20cm linewidth 4 rounded fontscale $my_fontscale
set output '$dir/$output'
load 'settings.gp'

set arrow from 0.5,0 to 0.5,1 as 90
set arrow from 0,0.5 to 1,0.5 as 90

set xrange [0:1]
set yrange [0:1]

set xtics 0.1
set ytics 0.1

set xlabel "$xlabel" 
set ylabel "$ylabel" 

#set  title '$title' offset -5,-14
set format x "%3.1f"

plot '$dir/$input' using '$x':'$y' w p ls $linestyle, \
     (x>0? x/((4*x)-1) : 1/0) ls 82, (x>0? x/((3*x)-1) : 1/0) ls 83 
     
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

set terminal pdfcairo dl $my_dashlength size 20cm,20cm linewidth 4 rounded fontscale $my_fontscale
load 'settings.gp'
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

    grep '[^[:blank:]]' $dir/$input > $dir/noblanks.dat    

    xticks_range=$(awk -v m=$xmax 'BEGIN { print (m / 6) }')
    
    if [[ $title == "Life+Drop" ]]; then
	linestyle=1
    elif [[ $title == "Drop" ]]; then
	linestyle=2
    elif [[ $title == "Jaara" ]]; then
	linestyle=3
    else
	linestyle=1
    fi
    
    echo $0 $FUNCNAME "$@"

    gnuplot <<_EOF_
set terminal pdfcairo dl $my_dashlength size 20cm,20cm linewidth 4 rounded fontscale $my_fontscale
set output '$dir/$output'
load 'settings.gp'

set xrange[$xmin:$xmax]
set yrange[$ymin:$ymax]

stats '$dir/$input' using "$x":"$y" nooutput 
max_x = STATS_pos_max_y
#max_y = GPVAL_DATA_Y_MAX
#set label 1 gprintf("Max = %g, Par=%g", max_y, max_x) at 2, min_y-0.2
set xtics $xticks_range
set ytics 0.1
set format x "%3.1f"

set xlabel "Selection Parameter"
set ylabel "F-Score"

#set title "$title" offset 3,-3

plot 0.5 ls 82, \
     '$dir/noblanks.dat' \
     using "$x":"$y" \
     w lp ls $linestyle
_EOF_
    #$dir/noblanks.dat
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
set terminal pdfcairo dl $my_dashlength size 20cm,20cm linewidth 4 rounded fontscale $my_fontscale
load 'settings.gp'
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

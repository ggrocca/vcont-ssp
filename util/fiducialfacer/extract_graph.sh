#!/bin/bash

point=${1};
point=`printf "%02d" $point`

count=`cat ${2}  | grep -P "\t$point\t" | cut -f 5 | wc -l`
nome=`cat ${2}  | grep -P "\t$point\t" | cut -f 3 | uniq`

echo $nome;


for ((i=1; i<=25; i++))
{
    c=`cat ${2} | grep -P "\t$point\t" | cut -f 5 | awk -vthreshold=$i '$1 < threshold' | wc -l`;
    res=` bc -l <<< $c/$count`
    echo $i $'\t' $res >> graphs/$point
}

gnuplot <<_EOF_
set terminal pdfcairo size 20cm,20cm linewidth 4 rounded 
set output 'graphs/$nome.pdf'
set title '$nome'
set xtics 1
set ytics 0.05
plot 'graphs/$point' with lines
_EOF_

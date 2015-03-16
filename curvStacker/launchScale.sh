#!/bin/bash



argsN=$#;
meshes=()

i=1;
for ((i=1; i<$((argsN-1)); i++))
do
    meshes=("${meshes[@]}" ${!i})
done



n_proc=${!i}


chunks=$(($argsN - 2))


a=$(($chunks/$n_proc)) 


echo "numero totale mesh: $chunks"
echo "numero processi: $n_proc"
echo "dimensione chunks: $a"

if (($((a*n_proc)) < $chunks))
then
    a=$((a+1))
fi


for ((i=0; i<$n_proc; i++))
do


    temp=${meshes[@]:0+$((i*a)):$a}
    n_m=$(grep -o " "  <<< "$temp" | wc -l)
    n_m=$((n_m+1))
        if (($(($((i*a))+a)) > $chunks))
    then
	if (($((i*a))>= $chunks))
	    then
	    break
	fi
	n_m=$(($chunks-$((i*a))))
    fi

    echo "./curvStacker -D $n_m ${meshes[@]:0+$((i*a)):$a} -d ${!argsN} -G 365 510 -C 30 -O 2 -c map$i -z height$i -L -200000 -t &"
    ./curvStacker -D $n_m ${meshes[@]:0+$((i*a)):$a} -d ${!argsN} -G 365 510 -C 30 -O 2 -c map$i -z height$i -L -200000 -t &
 done


wait
echo "DONE";

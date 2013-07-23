EXE=../scaletracker/scaletracker

DIFFTIFF="../analyzers/difftiff/difftiff -v 0"
DIFFTRAC="../analyzers/difftrack/difftrack -v 0"
DIFF="diff -q"

NEWRES=../../scalespace-results/mini
OLDRES=../../../horizon/smoother-results/mini

NUM_LEVELS=8

NORMAL1000="normal1000"
NORMAL1000_OLDOPTS="    -p 0.0001 0 -c 2500 2500 3500 3500 "
NORMAL1000_OPTS=" -m 14 -p 0.0001 0 -c 2500 2500 3500 3500 "
SMOOTH1000="smooth1000"
SMOOTH1000_OLDOPTS="    -p 0.0001 0 -c 2500 2500 3500 3500 -s "
SMOOTH1000_OPTS=" -m 14 -p 0.0001 0 -c 2500 2500 3500 3500 -s "
DATASETS="$NORMAL1000 $SMOOTH1000"

CRT=crt
TRK=trk
FILETYPES="$CRT $TRK"

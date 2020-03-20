#!/bin/sh
#
  while true
  do	  
    datum=`date +%y%m%d-%H`  # datum
    ./dcon -a 84 -i 60 -w 2 | tee -a data_${datum}.dat >/dev/null
    if [ -s data_${datum}.dat ]; then  # Soubor je nenulove delky
      tar cfz data_${datum}.tgz data_${datum}.dat && rm data_${datum}.dat
    fi
    sleep 10
  done 

#!/bin/sh
#
  device_name="PEC_H"

  while true
  do	  
    datum=`date +%y%m%d-%H`  # datum
    name="${datum}_${device_name}"
    ./dcon -a 01 -i 1 -w 2 -d -x ${device_name} -z 0,0 -z 1,0 | tee -a ${name}.dat >/dev/null
    if [ -s ${name}.dat ]; then  # Soubor je nenulove delky
      tar cfz ${name}.tgz ${name}.dat && rm ${name}.dat
    fi
#    sleep 10
  done 

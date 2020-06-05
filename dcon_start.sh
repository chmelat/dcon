#!/bin/sh
#
  device_name="PEC-HV"
  dest_dir="/mnt/pubkovy/m_data/"

  while true
  do	  
    datum=`date +%y%m%d%H`  # datum
    name="${device_name}_${datum}"
    ~/bin/dcon -a 01 -i 10 -w 1 -d -x ${device_name} -z 0,0 -z 1,0 | tee -a ${name}.dat >/dev/null
    if [ -s ${name}.dat ]; then  # Soubor je nenulove delky
      tar cfz ${name}.tgz ${name}.dat && rm ${name}.dat
      cp ${name}.tgz ${dest_dir}
    fi
#    sleep 10
  done 

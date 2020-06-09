#!/bin/sh
#
# V0.1/2020-06-09
# Simple datalogger for dcon
#
  device_name="PEC-HV"
  dest_dir="/mnt/pubkovy/m_data/"
#
# dcon with parameters directed to data file
  dcon_par () {
    ~/bin/dcon -a 01 -i 10 -w 2 -d -x ${1} -z 0,0 -z 1,0 |\
    tee -a ${2}.dat >/dev/null
  }
#
# Copy zipped  data to destination directory
  copy_dat () {
    if [ -s ${1}.dat ]; then  # Soubor je nenulove delky
      tar cfz ${1}.tgz ${1}.dat && rm ${1}.dat
      cp ${1}.tgz ${2}
    fi
  }

#
# main
  while true
  do
    datum=`date +%y%m%d%H`  # datum
    name="${device_name}_${datum}"

    dcon_par $device_name $name
    copy_dat $name $dest_dir &
  done

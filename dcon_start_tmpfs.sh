#!/bin/sh
#
# V0.1/2020-06-09
# Simple datalogger for dcon
#
  device_name="PEC-HV"
  data_dir="/mnt/tmpfs/"
  dest_dir="/mnt/pubkovy/m_data/"
#
# dcon with parameters directed to data file
  dcon_par () {
    ~/bin/dcon -a 01 -i 10 -w 1 -d -x ${1} -z 0,0 -z 1,0 |\
    tee -a ${3}${2}.dat >/dev/null
  }
#
# Copy zipped  data to destination directory
  copy_dat () {
    if [ -s ${1}.dat ]; then  # Soubor je nenulove delky
      cp ${3}.${1}.dat ${2}
      rm ${3}${1}.dat
    fi
  }

#
# main
  while true
  do
    datum=`date +%y%m%d%H%M`  # datum
    name="${device_name}_${datum}"

    dcon_par $device_name $name $data_dir
    copy_dat $name $dest_dir $data_dir &
  done

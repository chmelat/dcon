#!/bin/sh
#
# V0.1/2020-06-12
# V0.2/2020-08-03 add second dest. dir (dest_dir_s)
# Simple datalogger for dcon
#
  device_name="PEC-HV"
  data_dir="/mnt/tmpfs/"  # ramdisc
  dest_dir="/mnt/pubkovy/m_data/"
  dest_dir_s="/mnt/pubkovy/m_data_s/"

  cd /home/zero/dcon-PEC-HV  # working directory for read init scripts
#
# dcon with parameters directed to data file
  dcon_par () {
    dcon -a 01 -i 10 -w 1 -d -x $1 -z 0,0 -z 1,0 |\
    tee -a $2 >/dev/null
  }
#
# Copy data to destination directory
  copy_dat () {
    if [ -s $1 ]; then  # File is non zero length
      cp $1 $3
      cp $1 $2 && rm $1
    fi
  }
#
# main

  mv ${data_dir}*.dat ${dest_dir}

  while true
  do
    datum=`date +%y%m%d%H%M%S`  # date
    name="${data_dir}${device_name}_${datum}.dat"

    dcon_par $device_name $name
    copy_dat $name $dest_dir $dest_dir_s &
  done

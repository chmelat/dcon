#!/bin/sh
#
# Simple datalogger for dcon
# V0.1/2020-06-12
# V0.2/2020-08-03 Add second dest. dir (dest_dir_s)
# V0.3/2020-12-08 cd to working directory at top
#
  cd /usr/local/bin  # Working directory for read init scripts

  device_name="PEC-HV"
  data_dir="/mnt/tmpfs/"  # ramdisc
  dest_dir="/mnt/pubkovy/m_data/"
  dest_dir_s="/mnt/pubkovy/m_data_s/"
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

  if [ -d "$dest_dir" ]; then  # Destination directory exist
    cp ${data_dir}*.dat ${dest_dir}
    cp ${data_dir}*.dat ${dest_dir_s} && rm ${data_dir}*.dat
  fi  

  while true
  do
    datum=`date +%y%m%d%H%M%S`  # date
    name="${data_dir}${device_name}_${datum}.dat"

    dcon_par $device_name $name
    copy_dat $name $dest_dir $dest_dir_s &
  done

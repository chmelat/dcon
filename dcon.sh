#! /bin/sh
# /etc/init.d/dcon.sh

  program=`basename $0`

# Carry out specific functions when asked to by the system
  case "$1" in
    start)
      echo "Starting dcon ..."
      /usr/local/bin/dcon_start_tmpfs.sh &
      ;;
    stop)
      echo "Stopping dcon ..."
      pkill dcon
      ;;
    *)
      echo "Usage: /etc/init.d/${program} {start|stop}"
      exit 1
      ;;
  esac

  exit 0 

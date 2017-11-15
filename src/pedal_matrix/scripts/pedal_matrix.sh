#!/bin/bash

log_dir=/home/debian/pedal_matrix/logs
if [ ! -d "$log_dir" ]; then
    mkdir &log_dir
fi
time_stamp=`date +%s`
log_file=/home/debian/pedal_matrix/logs/$time_stamp.txt
echo "Starting Pedal Matrix..." &>> $log_file
echo "------------------------" &>> $log_file
sudo hciconfig hci0 down &>> $log_file
sudo hciconfig hci0 up &>> $log_file
cd /home/debian/pedal_matrix
sudo python3 __init__.py &>> $log_file
echo "Exiting Pedal Matrix..." &>> $log_file
echo "------------------------" &>> $log_file

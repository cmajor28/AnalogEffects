#!/bin/bash

log_dir=/home/debian/pedal_matrix/logs
if [ ! -d "$log_dir" ]; then mkdir $log_dir; fi
log_file=$log_dir/log.txt
echo "" > $log_file
echo "Starting Pedal Matrix..." >> $log_file
echo "------------------------" >> $log_file

# TODO this is a hack for gpio
sudo /home/debian/pedal_matrix/scripts/fix_gpio.sh &>> $log_file

sudo hciconfig hci0 down &>> $log_file
sudo hciconfig hci0 up &>> $log_file
cd /home/debian/pedal_matrix
if [ ! -d "data" ]; then mkdir data; fi
sudo python3 -u __init__.py &>> $log_file
echo "Exiting Pedal Matrix..." >> $log_file
echo "------------------------" >> $log_file

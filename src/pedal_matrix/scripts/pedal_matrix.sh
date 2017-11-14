#!/bin/bash

log_file=/home/debian/pedal_matrix
echo "Starting Pedal Matrix..." >>& $log_file
echo "------------------------" >>& $log_file
cd /home/debian/pedal_matrix
sudo hciconfig hci0 down &>> $log_file
sudo hciconfig hci0 up &>> $log_file
sudo python3 __init__.py &>> $log_file
echo "Exiting Pedal Matrix..." >>& $log_file
echo "------------------------" >>& $log_file

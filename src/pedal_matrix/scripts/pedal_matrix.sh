#!/bin/sh

cd /home/debian/pedal_matrix
sudo hciconfig hci0 down
sudo hciconfig hci0 up
sudo python3 __init__.py

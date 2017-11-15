#!/bin/bash

#sudo hciconfig hci0 down
#sudo hciconfig hci0 up
export DISPLAY=:0
xhost local:root
cd ../
sudo DISPLAY=$DISPLAY gdb python3

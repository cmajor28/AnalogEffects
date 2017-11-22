#!/bin/bash

echo "7" >> /sys/class/gpio/export
echo "112" >> /sys/class/gpio/export
echo "out" >> /sys/class/gpio/gpio7/direction
echo "out" >> /sys/class/gpio/gpio112/direction
echo "0" >> /sys/class/gpio/gpio7/value
echo "0" >> /sys/class/gpio/gpio112/value


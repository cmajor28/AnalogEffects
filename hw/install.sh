dtc -O dtb -i ./inc -o pedal_matrix-00A0.dtbo -b 0 -@ pedal_matrix-00A0.dts
cp pedal_matrix-00A0.dtbo /lib/firmware
if [ -f /boot/initrd.img-$(uname -r) ] ; then
    sudo update-initramfs -u -k $(uname -r)
else
    sudo update-initramfs -c -k $(uname -r)
fi
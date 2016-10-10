#!/bin/bash

#Stolen from pic32_reset.sh and modified for an argument for PIC32
# device-selection...

# call: 'pic32_reset2.sh <device>' 
#  where <device> matches the MCU option in xc32-gcc, e.g.
#  '32MX170F256B' or '32MX230F064B' (dropping the 'PIC')



# This is taken almost directly from kinsamanka at github.com:
#https://github.com/kinsamanka/PICnc-V2/wiki/OpenOCD-PIC32-Programmer

# His file was called pic32openocd

#TODO: I need to make sure to check his opensource licensing requests!


#TODO: Also need to look into making this device (not board) selectable...
#      e.g. via MCU...?
#      (currently I'm using a custom board.cfg)

#BRD_CFG=pic32mx150f128b.cfg


#echo \$TRST_GPIO > /sys/class/gpio/unexport 2>/dev/null
#echo \$TRST_GPIO > /sys/class/gpio/export
#echo "low" > /sys/class/gpio/gpio\${TRST_GPIO}/direction
#echo  "1" > /sys/class/gpio/gpio\${TRST_GPIO}/active_low

#openocd  -f interface/rpi.cfg -f board/\${BRD_CFG} &

# Run openocd, but keep the script running... ('&')
# adapter_khz was 4000 before, but always fell back to non-bulk write...
#  (with default openOCD0.90)
#  taking nearly 10 minutes / write!
# adapter_khz 400 says falling back to non-bulk write, but:
#  BUG: register 'a0' not found
# (weird?)
# A hacked version of openOCD-0.90 is installed, now...
#  It fast-writes *REALLY FAST*
# See MIPS_ETC/_PIC32_firstGo/openOCD/openOCD-0.90_hacked/
# (OR was it not actually flashing??? per heartbeatTest/4
#  reattempted -1, flickers again... definitely flashing...)

echo "Resetting PIC32"
openocd -f interface/ftdi/myFlyswatter.cfg -c "adapter_khz 400" -f board/PIC$1.cfg > /dev/null 2>&1 &

sleep 1

# Clever(?) piping... and nc instead of telnet...
(
echo reset
sleep 1
echo resume
sleep 1
echo shutdown
) | nc localhost 4444 > /dev/null

# It appears that nc keeps running until the exit command (in program) is
# executed, which causes the 'telnet' connection to drop *and* exits
# openOCD...
# So... I don't know, off-hand, how to use the same openocd session to
# *resume*...


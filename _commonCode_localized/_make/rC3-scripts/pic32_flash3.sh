#!/bin/bash

#Stolen from pic32_flash.sh and modified for an argument for PIC32
# device-selection...

# call: 'pic32_flash2.sh <file> <device>'
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

calledAs="$0"
calledAsDir="$(dirname "$0")"


if [ -z $1 ]
then
  echo "firmware file is missing!"
  echo "Usage: $0 <hexfile>"
  exit
fi


openOCD=$4

if [ -z $4 ]
then
   echo "defaulting to 'openocd' rather'n an override"
   openOCD=openocd
fi
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
$openOCD -f $calledAsDir/openOCDscripts/myFlyswatter.cfg -c "adapter_khz 400" -f $calledAsDir/openOCDscripts/PIC${2}.cfg $3 &

sleep 1

# Clever(?) piping... and nc instead of telnet...
(
echo 'reset init'
sleep 1
echo pic32mx unlock 0
sleep 1
echo reset init
echo program $1 reset exit
) | nc localhost 4444 > /dev/null

# It appears that nc keeps running until the exit command (in program) is
# executed, which causes the 'telnet' connection to drop *and* exits
# openOCD...
# So... I don't know, off-hand, how to use the same openocd session to
# *resume*...


#$openOCD -f interface/ftdi/myFlyswatter.cfg -c "adapter_khz 4000" -f board/PIC$2.cfg &

$openOCD -f $calledAsDir/openOCDscripts/myFlyswatter.cfg -c "adapter_khz 4000" -f $calledAsDir/openOCDscripts/PIC${2}.cfg &


sleep 1

(
 echo reset halt
 sleep 1
 echo resume
 sleep 1
 echo shutdown
) | nc localhost 4444 > /dev/null


#This doesn't work...
#echo resume | nc localhost 4444 > /dev/null
#echo exit   | nc localhost 4444 > /dev/null

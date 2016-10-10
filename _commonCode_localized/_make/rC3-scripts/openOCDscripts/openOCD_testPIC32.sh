#!/bin/bash

# Run this when you're trying out a new PIC32 device...
#
# DON'T change the PIC32MX170F256B.cfg bit, here...
# The point is that this script will INTENTIONALLY cause openOCD
# to report an *unexpected* TAPID which you then copy to the new
# device .cfg file...

echo "This will *intentionally* attempt to connect to the wrong device"
echo "which will then report the *actual* (aka 'unexpected') device ID"
echo " e.g.:"
echo " '...JTAG tap: pic32mx.cpu       UNEXPECTED: 0x05808053'"
echo " '...Error: JTAG tap: pic32mx.cpu  expected 1 of 1: 0x26610053'"
echo " SO: Take the 'UNEXPECTED:' ID: '0x05808053'"
echo " And enter this into your new <device>.cfg file"
echo " (Hit <Enter> to continue. CTRL-C to exit)"
read


openocd -f myFlyswatter.cfg -c "adapter_khz 400" -f PIC32MX170F256B.cfg


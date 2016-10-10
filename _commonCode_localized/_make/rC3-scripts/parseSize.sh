#!/bin/bash

echo "This doesn't do anything yet..."
exit

#The idea is to take the output from 'size' and parse it into something
# more legible...
# The pic32 size output is *really* in need of some parsing...
# lots of .data/.bss listings of zero-size
# so combine these...?
# Also, vectors could be merged
# Finally: addr should be in hex, frankly
# (and what about debug stuff?)

IFS="
"

while [ 1 ]
do

   read line || break

  


done

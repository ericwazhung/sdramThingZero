#!/bin/bash

#For code running heartbeat < v2.00, quite a few function-names have
# changed
# This, when run in a project-directory, should make the necessary
# replacements.


find ./ -name \*.[cChH] -exec vim_switch.sh getHeartRate heart_getRate {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh init_heartbeat heart_init {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh init_heartBeat heart_init {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh INIT_HEARTBEAT HEART_INIT {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh heartClear heart_clear {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh setHeartRate heart_setRate {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh set_heartBlink heart_blink {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh heartUpdate heart_update {} \;
find ./ -name \*.[cChH] -exec vim_switch.sh heartupdate heart_voidUpdate {} \;


echo ""
echo ""
echo ""
echo "**** NOTE ****"
echo "  heartPinInputPoll() is being replaced with heart_getButton()"
echo "  AS I RECALL, heartPinInputPoll() would return the actual value on the pin"
echo "  NOW: heart_getButton() returns the state of the button:"
echo "       TRUE if ON, FALSE if OFF"
echo ""
echo "  ***** WATCH OUT FOR e.g. if(!heartPinInputPoll()) *******"
echo "        (and even if(heartPinInputPoll()))"
echo ""
echo ""
echo ""
read


find ./ -name \*.[cChH] -exec vim_switch.sh heartPinInputPoll heart_getInput {} \;


find ./ -name \*.[cChH] -exec vim_switch.sh heart_getInput heart_getButton {} \;

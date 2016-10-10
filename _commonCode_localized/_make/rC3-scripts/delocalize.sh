#!/bin/bash

# TODO:
#   Search zipped versions for an identical match...?
#    (don't forget mehPL differences)
#   Disregard mehPL in diffing...?



# Most of reallyCommon2.mk's bash-stuff is handled in that file,
# But it's probably a good idea to make these things shell-scripts, 
#  instead...
# (they're too hard to modify, too much confusion with $ vs $$, etc.)

# 'make delocalize'
#  will cause the project to work with the centralized _commonCode 
#  directory
#
# This script attempts to copy files from _commonCode_localized/ to the 
# centralized _commonCode/ if they don't already exist there.


#.PHONY: delocalize
#delocalize:
#	@/bin/rm -f $(LOCAL_MK)
#	@echo "### BEST NOT MODIFY THIS FILE, use 'make (de)localize' instead" 
# ...		> $(SHARED_MK)	
#	@echo "# When this is 1, we'll use the files in " 
#ifdef CENTRAL_COMDIR




CENTRAL_COMDIR="$1"
LOCAL_COM_DIR="$2"
CENTRAL_COMDIR_FULL="$3"
appName="$4"

echo "### CHECKING whether local commonCode differs from central commonCode:"
echo " CENTRAL: '$CENTRAL_COMDIR'"
echo "          ('$CENTRAL_COMDIR_FULL')"
echo "  vs."
echo " LOCAL: '$LOCAL_COM_DIR'"


if [ ! -d "$CENTRAL_COMDIR" ]
then
   echo "*** The centralized commonCode directory specified does not exist:"
   echo "    Your makefile is set to "
   echo "    CENTRAL_COMDIR='$CENTRAL_COMDIR'"
   echo "                  ='$CENTRAL_COMDIR_FULL'"
   echo -n "    Would you like to create it? [y/n] "
   read yn
   if [ "$yn" == "y" ]
   then
      mkdir -p "$CENTRAL_COMDIR"
   else
      echo "*** Cannot delocalize without a centralized commonCode directory"
      echo "    Check the line 'CENTRAL_COMDIR=...' in your makefile"
      echo "    Or try again, and select 'y' when asked to create it."
      exit 1
   fi
fi

IFS="
"


unhandledCount=0
copyCount=0

diffOut="`diff -rwbBq "$LOCAL_COM_DIR" "$CENTRAL_COMDIR"`"

for line in $diffOut 
do 
#	echo "diffOut: '$line'" 
   if [ "${line#"Only in $CENTRAL_COMDIR"}" != "$line" ]
   then
      # Don't handle files/dirs only in CENTRAL
      # (e.g. commonCode/versions this project doesn't use)
      echo -n ""
   elif [ "${line#"Only in $LOCAL_COM_DIR"}" != "$line" ]
	then
      # DO handle stuff that only exists in LOCAL_COM_DIR
      #  These should be moved to the CENTRAL_COMDIR
      #  (e.g. when delocalizing a project on a new machine
      #   or on a machine which doesn't yet have these particular 
      #   commonThings/versions)

      itemSubdir="${line##"Only in $LOCAL_COM_DIR"}"
      itemSubdir="${itemSubdir%%:*}"
      itemSubdir="${itemSubdir#/}"
      echo "itemSubdir='$itemSubdir'"


		copyCount=$((copyCount+1)) 
#		echo "$a: $line" 
      item="$LOCAL_COM_DIR/$itemSubdir/${line##*: }"
#      echo "   $item"

      toDir="$CENTRAL_COMDIR/$itemSubdir/$(basename "$item")"
      echo " Copying '$item'"
      echo "   to '$toDir'"
      cp -ri "$item" "$toDir"
      if [ "$?" != "0" ]
      then
         echo "   From: '$line'"
      fi
   elif [ "${line%differ}" != "$line" ]
   then
      ## things which differ need to be handled interactively:
      # e.g. when working on bug-fixes in a commonThing, you might do-so in
      # in a localized copy... test it out in that project first, then
      # delocalize in order to move those changes to the central location
      centralItem="${line% differ}"
      centralItem="${centralItem#* and }"

      localItem="${line#Files }"
      localItem="${localItem% and *}"


      echo ""
      echo "### diff -wbB '$localItem' '$centralItem' :"
      diff -wbB "$localItem" "$centralItem"
      echo "###################################"
      echo "## diff reports that the files differ (see above):"
      echo "##  localItem: '$localItem'"
      echo "##  centralItem: '$centralItem'"

      which backup.sh > /dev/null
      if [ "$?" == "0" ]
      then
         useBackup=1
      else
         useBackup=0
      fi

      backupSuffix="delocalizing_${appName}_`date +%F_%Hh%Mm%Ss`"

      echo "##"
      echo "## Would you like to replace the centralized file with the local version?"
      echo "## NOTE: do-so carefully! This could break other programs that use it!"
      echo -n "## [y/n]"
      read yn

      if [ "$yn" == "y" ]
      then
         if [ "$useBackup" == "1" ]
         then
            backup.sh "$centralItem" -t="$backupSuffix"
         else
            mv -i "$centralItem" "${centralItem}_$backupSuffix"
         fi

         cp -ri "$localItem" "$centralItem"
      else
         echo "##  THESE LOCAL AND CENTRAL FILES WILL REMAIN UNCHANGED"
         echo "##  Handle their differences manually, as necessary"
         unhandledCount=$((unhandledCount+1))
      fi
      

   else
      echo "###################################################"
      echo "## NOT HANDLED: diffOut: '$line'"
      echo "###################################################"
      read pause
      unhandledCount=$((unhandledCount+1))
   fi 
done 


if [[ $unhandledCount > 0 ]] 
then 
	echo "#########################################################" 
	echo "## Unhandled Differences found! Be sure to check them! ##" 
	echo "#########################################################" 
	echo -n "  Do you want to continue delocalizing? [y/n]"
   read yn
   if [ "$yn" == "y" ]
   then
      echo "continuing delocalization..."
      exit 0
   else
      exit 1
   fi
elif [[ $copyCount > 0 ]]
then
   echo "...Copyover complete."
else
   echo "...No differences found, nothing to do."
fi

echo "continuing delocalization..."
exit 0




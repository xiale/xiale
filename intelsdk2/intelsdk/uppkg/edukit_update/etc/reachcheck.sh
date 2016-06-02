#!/bin/sh

APP="/usr/local/reach"
APPERROR="/var/log/recserver/reach/.errorapp"
TMPAPP="/var/log/recserver/reach/.tmpapp"
echo Reach Boot detection ...
if [ -f $APPERROR ] ;then
	echo ------------Reach Record App Error---------------
	echo ------------Start Repair program ----------------

	if [ -d $TMPAPP ] ;then
		if [ "`ls $TMPAPP`" != "" ];
		then
			rm -fr $APP
			mv  $TMPAPP $APP
			rm -rf $APPERROR
			echo  ---------------Repair Success!!!!-------------
		fi
	else
		echo ------------Repair Failure---------------
	fi
	rm -f $TMPAPP
fi

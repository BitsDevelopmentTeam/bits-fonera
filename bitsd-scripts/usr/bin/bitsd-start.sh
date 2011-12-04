#!/bin/sh

. /usr/lib/network.sh

connect

## Try once each day to get the date from the server
## This make sure we suport daylight saving time (ora solare/legale)
DATE=`wget -q -O - http://bits.poul.org/data.php`
if [ -n "$DATE" ]; then
	date -s "$DATE" ## Use "$DATE" as DATE contains a space
fi

## Before starting bitsd kill any running instance. This happens if
## the script bitsd-start.sh is called two or more times without calling
## bitsd-stop.sh in the middle
killall bitsd

su bits -c bitsd &

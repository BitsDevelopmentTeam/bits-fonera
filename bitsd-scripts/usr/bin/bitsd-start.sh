#!/bin/sh

. /usr/lib/network.sh

connect

## Try once each day to get the date from the server
## This make sure we suport daylight saving time (ora solare/legale)
setdate

startbits

#!/bin/sh

## File exists if connected
NETFLAG='/tmp/network-up'
DATESITE='http://bits.poul.org/data.php'

## Returns 0 if successful, 1 otherwise
setdate() {
	local DATE=`wget -q -O - $DATESITE`
	date -s "$DATE" ## Use "$DATE" as DATE contains a space
	return $?
}

checkroot() {
	if [ `id -u` != '0' ]; then
		echo 'You must be root'
		exit 1
	fi
}

is_connected() {
	[ -f "$NETFLAG" ]
	return $?
}

connect() {
	checkroot
	## Don't connect if already connected
	if [ -f "$NETFLAG" ]; then
		echo 'Already connected.'
		return
	fi
	echo 'Connecting...'
	touch $NETFLAG

	## Connect the Fonera to the Internet
	wlanconfig ath0 create wlandev wifi0
	ifconfig ath0 up
	iwconfig ath0 essid internet
	wpa_supplicant -i ath0 -D madwifi -c /etc/wpa_supplicant.conf -d 1>/dev/null 2>/dev/null &
	dhcpcd ath0
	sleep 40
	setdate
	openvpn --config /etc/openvpn/client.conf 1>/dev/null 2>/dev/null &
	## Without this long delay this function returns but the network isn't up yet,
	## and this causes an infinite loop in gettime-init
	sleep 120
	echo 'Done'
}

disconnect() {
	checkroot
	## Kill programs and disconnect
	killall openvpn dhcpcd wpa_supplicant
	ifconfig ath0 down
	wlanconfig ath0 destroy

	## Remove connected flag
	rm -f $NETFLAG
	echo 'Disconnected.'
}

startbits() {
	## Before starting bitsd kill any running instance. This happens if
	## the script bitsd-start.sh is called two or more times without calling
	## bitsd-stop.sh in the middle
	killall bitsd
	#TODO: adduser su bits -c "bitsd &"
	bitsd &
}

stopbits() {
	killall bitsd
}

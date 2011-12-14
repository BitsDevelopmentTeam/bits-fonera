#!/bin/sh

## File exists if connected
NETFLAG='/tmp/network-up'
DATESITE='http://bits.poul.org/data.php'

checkroot() {
	if [ `id -u` != '0' ]; then
		echo 'You must be root'
		exit 1
	fi
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
	sleep 30
	openvpn --config /etc/openvpn/client.conf 1>/dev/null 2>/dev/null &
	sleep 30
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

#!/bin/sh

. /usr/lib/gpio.sh

if [ -z "$1" ]; then
	echo "Usage $0 file.bin"
	exit 1
fi

trap ctrl_c INT

ctrl_c() {
	echo "Caught Ctrl-C, cleaning up..."
	leave_firmware_upgrade
}

config_gpios
enter_firmware_upgrade
stm32flash -w $1 -v /dev/ttyS0
leave_firmware_upgrade

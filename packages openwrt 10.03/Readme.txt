Readme for wpa_supplicant.ipk
=============================

This version of wpa_supplicant should work with wpa enterprise authentication.
Compiled for openwrt backfire 10.03.

First thing log through web gui, set root password and set eth0 ip address
to 192.168.0.1.

Then do an

opkg update
opkg remove luci*
opkg remove uhttpd
opkg remove wpad-mini
opkg install openvpn

install from opkg bitsd, bitsd-scripts, wpa_supplicant and stm32flash

vi /etc/init.d/cron
and set START=75

/etc/init.d/cron disable
/etc/init.d/cron enable
/etc/init.d/bitsdd enable
/etc/init.d/gettime-init enable

Lastly remove the line

tyS0::askfirst:/bin/ash --login

from /etc/inittab


#!/bin/sh

reset=7 # Gpio 7 is connected through a not gate to the stm32 reset
boot=3  # Gpio 3 is connected directly to the stm32 boot0

# To be called at boot to configure GPIOs
config_gpios() {
	gpioctl dirout $reset
	gpioctl dirout $boot
	gpioctl clear $boot
	gpioctl clear $reset
}

# After this function the stm32board is in firmware programming mode
enter_firmware_upgrade() {
	gpioctl set $reset
	gpioctl set $boot
	sleep 1
	gpioctl clear $reset
	sleep 1
	gpioctl clear $boot
}

# Restarts normal execution.
# If called without enter_firmware_upgrade it resets the board
leave_firmware_upgrade() {
	gpioctl set $reset
	gpioctl clear $boot
	sleep 1
	gpioctl clear $reset
}

#!/usr/bin/env python3

###############################################
#
# Communication with Arduino Mega over Serial
#
#
#
###############################################

import time
import serial
import struct
from pySerialTransfer import pySerialTransfer as txfer

StartMarker = "<"
EndMarker = ">"
SPEED_FL = 0
SPEED_FR = 0
SPEED_RL = 0
SPEED_RR = 0

def DataArduino(StartMarker,SPEED_FL,SPEED_FR,SPEED_RL,SPEED_RR,HEARTBEAT,EndMarker):
    
    return struct.pack(StartMarker,SPEED_FL,SPEED_FR,SPEED_RL,SPEED_RR,HEARTBEAT,EndMarker)
	

if __name__ == '__main__':
    link = txfer.SerialTransfer('/dev/ttyUSB0', 115200)
    link.open()
    time.sleep(2) # wait for arduino to reset

    while True:	
        DataArduino()
		link.write(DataArduino)
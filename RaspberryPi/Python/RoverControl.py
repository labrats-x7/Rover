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
SpeedFL = 0
SpeedFR = 0
SpeedRL = 0
SpeedRR = 0
Heartbeat = 0

def DataArduino(StartMarker,SpeedFL,SpeedFR,SpeedRL,SpeedRR,Heartbeat,EndMarker):
    return struct.pack(StartMarker,SpeedFL,SpeedFR,SpeedRL,SpeedRR,Heartbeat,EndMarker)
	

if __name__ == '__main__':
    link = txfer.SerialTransfer('/dev/ttyUSB0', 115200)
    link.open()
    time.sleep(2) # wait for arduino to reset

    while True:	
        DataArduino()
        link.write(DataArduino)

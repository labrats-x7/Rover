import time
import serial

filename = "/var/www/html/.send_command"

ser = serial.Serial('/dev/ttyUSB0', 115200)  # open serial port
print(ser.name)         # check which port was really used
while True:
	with open(filename) as f:
    		content = f.readlines()

	for line in content:
		print(line)
		ser.write(bytes(line,"ascii"))     # write a string
		print(ser.read(ser.in_waiting).decode("ascii"))
	time.sleep(0.1)
##########################################################################################
#
# Python 3 program for Rover-Control with Raspberry Pi Zero W and PS4 Dualshock Controller
#
# inspired by:
# https://www.elektronx.de/downloads/Rc-car-ps3.py
# https://www.pygame.org/docs/ref/joystick.html#pygame.joystick.Joystick.get_axis
# https://3.python-requests.org/user/quickstart/#make-a-request
#
#  0 - X
#  1 - O
#  2 - triangle
#  3 - square
#  4 - L1
#  5 - R1
#  6 - L2
#  7 - R2
#  8 - share
#  9 - options
# 10 - 
# 11 - analog L (defective)
# 12 - analog R
#
##########################################################################################

import pygame
import sys
import requests
import time

pygame.init()

j = pygame.joystick.Joystick(0)
j.init()
speed = 0
servo = 0
nonce = 0

try:
	with requests.Session() as s:	
		while j.get_button(10) == 0:	#PS Button  -> Exit
			pygame.event.pump()
			R2 = j.get_axis(5)
			L2 = j.get_axis(2)
			nonce = nonce + 1
			if nonce > 1023:
				nonce = 1
			#Speed (enable motors with L1)
			if j.get_button(4) != 0.00:
				speed = ((R2 + 1) - (L2 + 1)) * 512
			elif j.get_button(4) == 0.00:
				speedR = 0
				speedL = 0
				print("DISABLE   servo= ",servo," speedL= ",speedL," speedR= ",speedR)
			#Steer (enable skid steer with R1)
			if j.get_button(5) == 0 and j.get_button(4) != 0:
				steer = j.get_axis(3)
				servo = steer * 128
				speedR = speed
				speedL = speed
				print("SERVOMODE servo= ",servo," speedL= ",speedL," speedR= ",speedR)
			elif j.get_button(5) != 0 and j.get_button(4) != 0:
				steer = j.get_axis(3)
				servo = 0.0
				speedR = speed*0.75-(steer*256)
				speedL = speed*0.75+(steer*256)
				print("SKID-MODE servo= ",servo," speedL= ",speedL," speedR= ",speedR)
			#camera movement and zoom with hat and share/option
			hat=j.get_hat(0)
			lr, ud = hat
			if lr < 0:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=left',timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass
			if lr > 0:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=right', timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass
			if ud < 0:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=up', timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass
			if ud > 0:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=down', timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass	
			if j.get_button(8) == 1:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&rzoom=-2500', timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass
			if j.get_button(9) == 1:
				try:
					s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&rzoom=+2500', timeout=0.001)
				except requests.exceptions.RequestException:
					print("n schwarzen Bildschirm hab ich!")
					pass
			#send speed and steer to set.php
			seq = ('<', str(int(speedL)), str(int(speedR)), str(int(speedL)), str(int(speedR)), str(int(servo)), str(nonce), '>')
			send = ','.join(seq)
			payload = {'v':send}
			try:
				r = s.get('http://127.0.0.1/set.php?', params=payload, timeout=3.05)
			except requests.exceptions.RequestException:
				print("failed to communicate with set.php")
				pass
			#print(r.url)
			time.sleep(0.1)
except KeyboardInterrupt:
	j.quit()
	sys.exit()
import RPi.GPIO as GPIO
import time

# Set GPIO numbering mode to match pin 7 on the board to the GPIO with 50hz pulse length
GPIO.setmode(GPIO.BOARD)
GPIO.setup(11,GPIO.OUT) 
servo_steer = GPIO.PWM(11,50)

servo_steer.start(0)


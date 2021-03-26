# Howaran
Arduino Mega code for communicating with 2 hoverboards from a raspberry pi in /Arduino

Python code for Communicating between Arduino Mega and Raspberry Pi in /RaspberryPi/Python

index.html for mobile website for inputs

see hoverboard-firmware-hack-FOC repo for used hoverboard-firmware


---
## ToDo:

Mechanical:

    - Build upper deck
    - design camera/lidar mnount
    - cut Plexiglass walls
    - shorten the 4 outer screws on the front axle to correct the tire track with
    - model charge contacts


Electrical:
    
    - Cable extensions for front axle
    - DC/DC converter 5V 12V...
    - terminal blocks for power distribution
    - LiFePo batteries
    - Circuit diagram!
    - WS2812 strip (in extrusion slot?)
    - charge contacts
    - schottky diodes


Hoverboards:

    - firmware brake enable?
    - test modes
    - implement mode switching? (https://github.com/EmanuelFeru/hoverboard-firmware-hack-FOC/issues/143)
    - test field weakening 

Arduino:

    - servo pwm output -> from raspi?
    - no servo movement in standstill
    - safe speed mode (max. 1m/s)


Raspberry:

    Python:
        - WS2812

    WWW:
        - single-user-check einbauen (mit Rückmeldung)
        - Drive mode selector (tank/ackermann)
        - Wheel speed calculi
        - Telemetry (Speed, Battery, ...)
        - Camera feed
        - camera control
        - PS4 Controller input
        - insane speed mode toggle
        - speed adaptive steer angle
        - expo/rate
        - deadzone
        - edge fallback (data reduction) 


    Network:


---
## Data Connections
 
![data_connections](/docs/pictures/data_connections.png)
---


## Mechanics
Overview over the Rover geometry build from 40mm ITEM extrusions:

Iso:
![mechanics_iso](/docs/pictures/mechanics_iso.png)
Front:
![mechanics_front](/docs/pictures/mechanics_front.png)
Side:
![mechanics_side](/docs/pictures/mechanics_side.png)
---

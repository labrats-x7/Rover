##
Arduino Mega code for communicating with 2 hoverboards from a raspberry pi in /Arduino

Python code for Communicating between Arduino Mega and Raspberry Pi in /RaspberryPi/Python

see hoverboard-firmware-hack-FOC repo for used hoverboard-firmware


---
## ToDo:

Hoverboards:
    - firmware brake enable

Arduino:
    - nonce
    - safe speed mode


Raspberry:

    Python:
        - Git
        - Autostart

    WWW:
        - Speed/Steer tauschen
        - single-user-check einbauen (mit Rückmeldung)
        - nonce einbauen, auf dem UC checken if nonce gesehen > x mal -> use failsafe alias <0,0,0,0,0,0>
        - Servo value
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

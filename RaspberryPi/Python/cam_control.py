# ######################################################################
# python code to control the axis 214 ptz camera
# 
#
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&backlight=on (off)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&ircutfilter=auto (on off)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&autofocus=on (off)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=up  (down right left)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&tiltbar=194,vertical&barcoord=?0,200 (0-200)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&panbar=194,horisontal&barcoord=?200,0 (0-200)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&rzoom=-2500 (-xy  +xy)
# http://root:root@192.168.0.90/incl/specialzoomcmd.shtml?camNo=1&zoombar=178&coord=?50,0 (0-125, 126-177)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&rfocus=-2500 (-xy  +xy)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&focusbar=178,horisontal&barcoord=?49,0 (0-177)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&riris=-250 (-xy +xy)
# http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&irisbar=178,horisontal&barcoord=?147,0 (0-177)
# 
# video feed: http://192.168.0.90/mjpg/video.mjpg
#
#
#
## ######################################################################

import time
import requests

i=0

# Use 'with' to ensure the session context is closed after use.

with requests.Session() as s:
    p = s.get('http://root:root@192.168.0.90/view/viewer_index.shtml')
    # print the html returned or something more intelligent to see if it's a successful login page.
    print(p.text)

    while True:

        i = i+1
        if i > 20:
            i=0
            s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=right')
        if i == 10:
            s.get('http://root:root@192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=left')
        time.sleep(1)

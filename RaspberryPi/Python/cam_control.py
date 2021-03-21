# ######################################################################
# python code to control the axis 214 ptz camera
# 
# https://stackoverflow.com/questions/11892729/how-to-log-in-to-a-website-using-pythons-requests-module
#
#
#
# 
# ######################################################################

import time
import requests

i=0

# Fill in your details here to be posted to the login form.
payload = {
    'inUserName': 'root',
    'inUserPass': 'root'
}

# Use 'with' to ensure the session context is closed after use.
with requests.Session() as s:
    p = s.post('http://192.168.0.90/view/viewer_index.shtml', data=payload)
    # print the html returned or something more intelligent to see if it's a successful login page.
    print(p.text)

    while True:
        
        i = i+1
        if i > 20:
            i=0
            s.get('192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=right')
        if i == 10:
            s.get('192.168.0.90/axis-cgi/com/ptz.cgi?camera=1&move=left')
        time.sleep(1)
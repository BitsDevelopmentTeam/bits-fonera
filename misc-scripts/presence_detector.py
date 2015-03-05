#!/usr/bin/python

import subprocess
import re
import time
import hmac
import hashlib
import urllib
import json

KEY = "Your key here"
POST_PASSWORD = "Your password here"
SUBMIT_URL = "https://bits.poul.org/macupdate"

def getMacList():
    leases = open("/var/lib/misc/dnsmasq.leases","r").read()
    ips = re.findall("((?:[0-9]{1,3}\.){3}[0-9]{1,3})",leases)
    fd = open("/tmp/scaniplist.tmp","w")
    for ip in ips:
        fd.write(ip+"\n")
    fd.close()
    stuff = subprocess.check_output(["nmap","-T2","-sP","-PR","-n","-iL","/tmp/scaniplist.tmp","-oX","-"]).lower()
    macs = re.findall("((?:[a-f0-9]{2}:){5}[a-f0-9]{2})", stuff)
    return macs

while True:
    print("Starting scan")
    args = { "macs" : [],
             "password" : POST_PASSWORD
            }
    for mac in getMacList():
        args["macs"].append(hmac.new(KEY, mac, hashlib.sha256).hexdigest())

    args["macs"] = json.dumps(args["macs"])
    page = urllib.urlopen(SUBMIT_URL, urllib.urlencode(args)).read()
    print("Submission completed")
    time.sleep(60*5)


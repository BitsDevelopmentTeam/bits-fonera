#!/usr/bin/python

import subprocess
import re
import time
import hmac
import hashlib
import urllib
import json
from scapy.all import *

KEY = "Your key here"
POST_PASSWORD = "Your password here"
SUBMIT_URL = "Your submission url here"

# Output interface to use for the ping (note: the all nodes special multicast
# address requires to specify an output interface)
OUT_IF = "eth1"

# Scan every 1 minute
SCAN_PERIOD = 60

def getMacList():
    # We exploit the all-nodes multicast ipv6 address for pinging and
    # getting a reply from all hosts connected to poul's network.

    macs = set() # We want to avoid returning the same mac multiple times

    ans,unans = srp(Ether(dst="33:33:00:00:00:01")/IPv6(
                    dst="ff02::1")/ICMPv6EchoRequest(),
                    timeout = 3, multi = 1, iface = OUT_IF)

    for replies in ans:
        macs.add(replies[1].src)

    return macs

while True:
    print("Starting scan")

    args = { "macs" : [],
             "password" : POST_PASSWORD
            }

    for mac in getMacList():
        args["macs"].append(hmac.new(KEY, mac, hashlib.sha256).hexdigest())

    print("{} MACs found".format(len(args["macs"])))

    # Convert the list into a json string
    args["macs"] = json.dumps(args["macs"])

    # Send the POST
    try:
        page = urllib.urlopen(SUBMIT_URL, urllib.urlencode(args)).read()
        print("Submission completed")
    except IOError:
        print("Submission failed")

    time.sleep(SCAN_PERIOD)


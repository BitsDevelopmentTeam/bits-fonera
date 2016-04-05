#!/usr/bin/python

import subprocess
import re
import time
import hmac
import hashlib
import urllib
import json
import ipaddr

KEY = "Your key here"
POST_PASSWORD = "Your password here"
SUBMIT_URL = "https://bits.poul.org/macupdate"

# Output interface to use for the ping (note: the all nodes special multicast
# address requires to specify an output interface)
OUT_IF = "eth1"

# Scan every 1 minute
SCAN_PERIOD = 60

def get_mac_addr(addr):
    """ Take out MAC address bytes from the IPv6, and return the MAC
        address in a string format.
    """
    mac_bytes = []
    mac_bytes.append(int(((int(addr) >> 56) & 0xff) ^ 0x02))
    mac_bytes.append(int((int(addr) >> 48) & 0xff))
    mac_bytes.append(int((int(addr) >> 40) & 0xff))
    mac_bytes.append(int((int(addr) >> 16) & 0xff))
    mac_bytes.append(int((int(addr) >> 8) & 0xff))
    mac_bytes.append(int(int(addr) & 0xff))
    return ":".join([hex(x)[2:].zfill(2) for x in mac_bytes])

def getMacList():
    # We exploit the all-nodes multicast ipv6 address for pinging and
    # getting a reply from all hosts connected to poul's network.
    ping_output = subprocess.check_output(["ping6", "-I", OUT_IF, "-w", "6",
                                           "-i", "2", "ff02::1"]).lower()
    ipv6s = re.findall("64 bytes from (.*): icmp_seq", ping_output)

    macs = set() # We want to avoid returning the same mac multiple times
    for addr_str in ipv6s:
        try:
            addr = ipaddr.IPAddress(addr_str)
        except ValueError:
            pass # Skipping invalid address

        if addr.is_link_local:
            macs.add(get_mac_addr(addr))
        
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



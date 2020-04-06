import os
import platform
import json
import argparse

parser = argparse.ArgumentParser(description='deploy to specified ports')
parser.add_argument('devices', metavar='-D', type=str, nargs='+',
                   help='Device serial port')
parser.add_argument('--monitor', metavar='-m', type=str, nargs=1)

args = parser.parse_args()

if(args.devices):
    for device in args.devices:
        print(device)
        port = device
        os.system("pio run --target upload --upload-port "+ port)
elif(platform.system() == "Windows"):
    stream = os.popen("pio device list --json-output")
    output = stream.read()
    devices = json.loads(output)
    # print(devices[0]['port'])
    for device in devices:
        print(device["port"])
        port = device["port"]
        os.system("pio run --target upload --upload-port "+ port)

if(args.monitor):
    print("Monitoring port" + args.monitor[0])
    os.system("pio device monitor "+args.monitor[0])


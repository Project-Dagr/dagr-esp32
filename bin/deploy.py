import os
import platform
import json

if(platform.system() == "Windows"):
    stream = os.popen("pio device list --json-output")
    output = stream.read()
    devices = json.loads(output)
    # print(devices[0]['port'])
    for device in devices:
        print(device["port"])
        port = device["port"]
        os.system("pio run --target upload --upload-port "+ port)


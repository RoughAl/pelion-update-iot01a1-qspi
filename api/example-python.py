import os
import pprint
from mbed_cloud import ConnectAPI

TOKEN = "YOUR_ACCESS_KEY"

# set up the Python SDK
config = {}
config['api_key'] = os.getenv('TOKEN') or TOKEN
config['host'] = 'https://api.us-east-1.mbedcloud.com'
api = ConnectAPI(config)
api.start_notifications()

devices = list(api.list_connected_devices(filters={'device_type': 'default'}))

print("Found %d devices" % (len(devices)), [ c.id for c in devices ])

for device in devices:
    def pir_callback(device_id, path, distance):
        print("Distance changed at %s, new value is %s" % (device_id, distance))

    api.add_resource_subscription_async(device.id, '/3330/0/5700', pir_callback)
    print("subscribed to resource")

    api.execute_resource(device.id, '/3201/0/5850', '1')
    print("blinked LED of %s" % device.id)

# Run forever
while True:
    pass
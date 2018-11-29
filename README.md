# Mbed OS and Arm Pelion workshop with IDB01A1

## Prerequisites

1. Create an Arm Mbed online account [here](https://os.mbed.com/account/signup/).
1. Register for [Pelion access](https://console.mbed.com/cloud-registration).
1. Then install the following software for your operating system below:

**Windows**

If you are on Windows, install:

1. If you're on Windows XP, 7 or 8: [ST Link](http://janjongboom.com/downloads/st-link.zip) - serial driver for the board.
    * Run `dpinst_amd64` on 64-bits Windows, `dpinst_x86` on 32-bits Windows.
    * Afterwards, unplug your board and plug it back in.
    * (Not sure if it configured correctly? Look in 'Device Manager > Ports (COM & LPT)', should list as STLink Virtual COM Port.
1. [Tera term](https://osdn.net/projects/ttssh2/downloads/66361/teraterm-4.92.exe/) - to see debug messages from the board.

**Linux**

If you're on Linux, install:

1. screen - e.g. via `sudo apt install screen`

**MacOS**

Nothing required.

## 0. Setting up the Online Compiler

1. Go to the [DISCO-L475VG-IOT01A1](https://os.mbed.com/platforms/ST-Discovery-L475E-IOT01A/) platform page.
1. Click **Add to your Mbed Compiler**.
1. Click **Open Mbed Compiler**.
1. The Online Compiler opens.
1. Click **Cancel** in the 'Create new program' wizard.
1. Click **Import** > **Click here to import from URL**.
1. Enter `https://github.com/janjongboom/pelion-update-iot01a1-qspi`
1. Click **Import**.
    * Do **NOT** click 'Update all libraries...'

## 1. Blinky

1. In `select-demo.h` select `DEMO_1`.
1. Open `1_mbedos\1_main.cpp`.
1. Click **Compile**.
1. A file downloads, drag it to the DIS_L4IOT disk.
1. LED flickers red/yellow.
1. After it stops your board is programmed.
1. The built-in LED on your board will blink every 0.5 seconds.

**Windows**

1. Open Tera Term.
1. Select STLink Virtual COM port.
1. Under Serial, select baud rate 115,200.
1. You should see output.

**macOS**

1. Open a terminal.
1. Run:

    ```
    $ ls /dev/ | grep tty.usbm
    tty.usbmodem14503
    ```

1. Take the output from the previous command and run:

    ```
    $ screen /dev/tty.usbmodem14503 115200
    ```

To exit, press: `CTRL+A` then `CTRL+\` then press `y`.

**Linux**

1. Open a terminal.
1. Run:

    ```
    $ ls /dev/ttyACM*
    /dev/ttyACM0
    ```

1. Take the output from the previous command and run:

    ```
    sudo screen /dev/ttyACM0 115200                # might not need sudo if set up lsusb rules properly
    ```

To exit, press `CTRL+A` then type `:quit`.

### 1.1 Adding sensors

The board you have is equipped with a lot of sensors. Let's get some data from the temperature sensor. Under `#include`'s, add:

```cpp
#include "HTS221Sensor.h"
```

And replace the beginning of `main` with:

```cpp
int main()
{
    static DevI2C devI2c(PB_11, PB_10);
    static HTS221Sensor hum_temp(&devI2c);
    hum_temp.init(NULL);
    hum_temp.enable();

    while (true) {
        float temperature = 0.0;
        if (hum_temp.get_temperature(&temperature) == 0) {
            printf("Temperature is %.2f C\n", temperature);
        }
        else {
            printf("Failed to read temperature\n");
        }
```

There's also a humidity sensor on the board, can you enable that one?

### 1.2 Adding a new thread

Mbed OS contains an RTOS and can spawn new threads. You do this through

```cpp
void new_thread_main() {
    while (1) {
        // main loop for the new thread
    }
}

int main() {
    Thread new_thread;
    new_thread.start(&new_thread_main);

    while (1) {
        // rest of your code
    }
}
```

Rewrite the code so that the temperature sensor is read in a separate thread. Then look at the output. What do you see? Do you see the new thread?

**Assignment:** we use only a fraction of the stack of the new thread, but have allocated 4096 bytes for it. Make the stack size of the new thread smaller. [Here's the documentation](https://os.mbed.com/docs/v5.8/reference/thread.html).

### 1.3 Using event queues

Mbed OS can also use event queues for scheduling, this is a very useful construct to do complex scheduling in a single thread. You can do things like:

```cpp
void read_temperature() {
    // temperature reading code
}

void blink_led() {
    // led blinking code
}

int main() {
    EventQueue ev_queue;

    ev_queue.call_every(500, &blink_led);
    ev_queue.call_every(750, &read_temperature);
    ev_queue.call_every(1000, &print_stats);

    ev_queue.dispatch_forever();
}
```

Event queues can also be used to debounce from one context to another. E.g. a `Ticker` triggers interrupts, which you normally need to debounce if you don't want to run in an interrupt context. Event queues can help with this, e.g. via:

```cpp
Ticker ledTicker;
ledTicker.attach(0.5f, ev_queue.event(&blink_led));
```

More information in [this blog post](https://os.mbed.com/blog/entry/Simplify-your-code-with-mbed-events/).

## 2. Arm Pelion

### 2.1 Building

1. Click **Import > Import from URL**.
1. Enter `https://github.com/coisme/Pelion-DM-Workshop-Project/` and click **Import**.
1. Open `mbed_app.json` and find SSID / Password declarations and fill them with the right keys (see the white board).
1. We need to add some certificates to this application:
    1. Click **Pelion Device Management > Manage Connect certificates**.
    1. Click on the link to create an API key.
    1. Click **Log in with your Mbed account**.
    1. Accept the license.
    1. Create a new API key. Make sure to give Administrator access to the key, and save it somewhere locally, we'll need it later.
    1. Paste the key in the Online Compiler.
    1. In the 'Certificate' window, click **Create**.
    1. Give the cert a name and click **OK**.
    1. Select the certificate, and click **OK**.
    1. Click **OK** again to overwrite the current certificate.
1. Next, we need an update public/private key pair for firmware updates.
    1. Click **Pelion Device Management > Apply Update Certificate**.
    1. Click **Create**.
    1. Click **Create** again to overwrite the current credentials.
    1. Click **Download private key** and store the key somewhere safe.
1. Click **Compile** to compile the application.
1. Flash the application to your development board.

**Note:** You can clear out the internal flash (required if at some point you get a certificate error) by pressing the BLUE button on the dev board during boot.

## 2.2 Pelion Device Management Portal

1. Head to the [Pelion DM Portal](https://portal.mbedcloud.com/dashboard/usage).
1. Find your device under the Device directory.
1. Go to Resources.
1. See if you can get the button count, and a way to blink the LED in a certain pattern.

But... we're missing most of our sensors! Let's fix this.

## 2.3 A firmware update

Open `main.cpp` and uncomment line 29.

1. Click **Pelion Device Management > Publish Firmware Update**.
1. When prompted, enter a name for the update, and click **Publish**.
1. Find the private key which downloaded earlier, and click **OK**.
1. Click **Open Device Management Portal**.
1. Click **Next**.
1. Select the name you just entered under 2) and click **Next**.
1. Select your device, and click **Next**.
1. Click **Finish**.
1. Click **Start**.

Watch the output on the serial port.

**UpdateWarningCertificateNotFound**

If you see the error above:

1. Go into `main.cpp`, change `*2` into `*8` on line 52 and compile.
1. Flash the application and start up while pressing the blue button.
1. A message should pop up telling you about reformatting of the storage device.
1. Change line 52 back and flash again.
1. Now retry the update.

## 3. API

The Device Management Portal that you used in the previous section is a wrapper around the Device Management API. Through this API, you can connect any app to any device.

### Obtaining an access key

To talk to the API, you need an API key. This key is used to authenticate with the API. To create a new access key, go to the [Access management](https://portal.mbedcloud.com/access/keys) page in the Device Management Portal.

Click **New API Key** to create a new API key, and name it.

### Testing the API

You can quickly test if the access key works by sending a call to the API to query for all the devices. To retrieve a list of all devices, make a GET request to `https://api.us-east-1.mbedcloud.com/v2/endpoints`. You need to send an authorization header with this request:

```
Authorization: Bearer <your_access_key>
```

You can make this request with any request library, but if you're using curl, use the following command:

```
curl -v -H "Authorization: Bearer <your_access_key>" https://api.us-east-1.mbedcloud.com/v2/endpoints
```

It will return something like this:

```
*   Trying 52.1.229.179...
* Connected to api.us-east-1.mbedcloud.com (52.1.229.179) port 443 (#0)
* TLS 1.2 connection using TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
> GET /v2/endpoints HTTP/1.1
> Host: api.us-east-1.mbedcloud.com
> User-Agent: curl/7.43.0
> Accept: */*
> Authorization: Bearer ak_...
>
< HTTP/1.1 200 OK
< Content-Type: application/json; charset=utf-8
< Server: nginx
< Content-Length: 85
< Connection: keep-alive
<
[
    {
        "name": "015b58400ce40000000000010010022a",
        "type": "default",
        "status": "ACTIVE"
    }
]
```

<span class="notes">**Note:** Please see the official [API documentation](https://cloud.mbed.com/docs/current/service-api-references/index.html) for the Device Management REST API interface.</span>

### Using the official libraries

Official Device Management SDKs are available for Node.js and Python. These APIs are asynchronous because for many functions, an action (such as writing to a device) might not happen immediately - the device might be in deep sleep or otherwise slow to respond. Therefore, you need to listen to callbacks on a notification channel. The official libraries abstract the notification channels and set up the channels for you, which makes it easier for you to write applications on top of Device Management.

An additional feature of the libraries is that they support subscriptions. You can subscribe to resources and get a notification whenever they change. This is useful for the `/3330/0/5700` (laser distance) resource because you can receive a notification whenever someone moves in front of the sensor.

The following sections show an example of toggling the LED blink function and receiving a notification whenever someone waves in front of the PIR sensor, in both Node.js and Python.

#### Node.js

First, make sure you have installed [Node.js](http://nodejs.org). Then, create a new folder, and install the Device Management Node.js SDK via npm:

```bash
$ npm install mbed-cloud-sdk --save
```

Next, create a new file `main.js` in the same folder where you installed the library, and fill it with the following content (replace `YOUR_ACCESS_KEY` with your access key):

```js
var TOKEN = 'YOUR_ACCESS_KEY';

var mbed = require('mbed-cloud-sdk');
var api = new mbed.ConnectApi({
    apiKey: process.env.TOKEN || TOKEN,
    host: 'https://api.us-east-1.mbedcloud.com'
});

// Start notification channel (to receive data back from the device)
api.startNotifications(function(err) {
    if (err) return console.error(err);

    // Find all devices
    var filter = { deviceType: { $eq: 'default' } };
    api.listConnectedDevices({ filter: filter }, function(err, resp) {
        if (err) return console.error(err);

        var devices = resp.data;
        if (devices.length === 0) return console.error('No devices found...');

        console.log('Found', devices.length, 'devices', devices.map(d => d.id));

        devices.forEach(function(d) {

            // Subscribe to the PIR sensor
            api.addResourceSubscription(
                d.id,
                '/3330/0/5700',
                function(distance) {
                    console.log('Distance changed at', d.id, 'new value is', distance);
                },
                function(err) {
                    console.log('subscribed to resource', err || 'OK');
                });

            // Set the color of the light
            api.executeResource(d.id, '/3201/0/5850', '1', function(err) {
                console.log('toggled LED blink function', err || 'OK');
            });

        });
    });
});
```

When you run this program and you wave your hand in front of the PIR sensor, you will see something like this:

```
$ node main.js
Found 1 device [ '015b58400ce40000000000010010022a' ]
subscribed to resource OK
toggled LED blink function OK
Distance changed at 015b58400ce40000000000010010022a new value is 131
Distance changed at 015b58400ce40000000000010010022a new value is 121
```

See the [full docs](https://cloud.mbed.com/docs/current/mbed-cloud-sdk-javascript/) on how to use the JavaScript SDK.

#### Python

1. Install [Python 2.7](https://www.python.org/downloads/) and [pip](https://pip.pypa.io/en/stable/installing/) if you have not already.
1. Create a new folder.
1. Install the Device Management SDK through pip:

**Windows, Linux**
```bash
$ pip install mbed-cloud-sdk
```
**MacOS**

```bash
$ pip install mbed-cloud-sdk --user python
```
**The remaining steps are the same regardless of which OS you use.**

1. Create a new file - `example-python.py` - in the same folder where you installed the library, and fill it with the following content (replace `YOUR_ACCESS_KEY` with your access key):

```python
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
```

When you run this program and you wave your hand in front of the PIR sensor, you see something like this:

```
$ python example-python.py
('Found 1 devices', ['015b58400ce40000000000010010022a'])
subscribed to resource
blinked LED of 015b58400ce40000000000010010022a
Distance changed at 015b58400ce40000000000010010022a, new value is 112
Distance changed at 015b58400ce40000000000010010022a, new value is 120
```

See the [full docs](https://cloud.mbed.com/docs/current/mbed-cloud-sdk-python/index.html) on how to use the Python library.

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


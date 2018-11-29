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
1. In the Online Compiler, click **Import** > **Import from URL**.
1. Enter `https://github.com/janjongboom/pelion-update-iot01a1-qspi`
1. Click **Import**.

## 1. Blinky

In `select-demo.h` select `DEMO_1`.

1. Click **Compile**.
1. A file downloads, drag it to the DIS_L4IOT disk.
1. LED flickers red/yellow.
1. After it stops your board is programmed.

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

The board you have is equipped with a lot of sensors.

### 1.1 Adding a new thread

You can run the stats tracking code in a separate thread. Replace the `int main()` function with:

```cpp
void new_thread_main() {
    while (1) {
        print_stats();

        Thread::wait(2000);
    }
}

int main() {
    Thread new_thread;
    new_thread.start(&new_thread_main);

    while (1) {
        led1 = !led1;

        Thread::wait(1000);
    }
}
```

Flash this on the board and compare with the previous output. What do you see? Do you see the new thread?

**Assignment:** we use only a fraction of the stack of the new thread, but have allocated 4096 bytes for it. Make the stack size of the new thread smaller. [Here's the documentation](https://os.mbed.com/docs/v5.8/reference/thread.html).

### 1.2 Using event queues

Mbed OS can also use event queues for scheduling, replace the previous code with:

```cpp
void blink_led() {
    led1 = !led;
}

int main() {
    EventQueue ev_queue;

    ev_queue.call_every(500, &blink_led);
    ev_queue.call_every(1000, &print_stats);

    ev_queue.dispatch_forever();
}
```

Event queues can also be used to debounce from one context to another. E.g. a `Ticker` triggers interrupts, which you normally need to debounce if you don't want to run in an interrupt context. Event queues can help with this, e.g. via:

```cpp
Ticker ledTicker;
ledTicker.attach(0.5f, ev_queue.event(&blink_led));
```

#include "select-demo.h"
#if SELECT_DEMO == DEMO_1


#include "mbed.h"
#include "stats_report.h"

static DigitalOut led1(LED1);

int main()
{
    while (true) {
        // Blink LED and wait 0.5 seconds
        led1 = !led1;
        wait(0.5f);

        // Following the main thread wait, report on the current system status
        print_stats();
    }
}

#endif // SELECT_DEMO

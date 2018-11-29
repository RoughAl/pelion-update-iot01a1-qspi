/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STATS_REPORT_H
#define STATS_REPORT

#include "mbed.h"

void print_stats() {
    // allocate enough room for every thread's stack statistics
    int cnt = osThreadGetCount();
    mbed_stats_stack_t *stats = (mbed_stats_stack_t*) malloc(cnt * sizeof(mbed_stats_stack_t));

    cnt = mbed_stats_stack_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++) {
        printf("Thread: 0x%lX, Stack size: %lu / %lu\r\n", stats[i].thread_id, stats[i].max_size, stats[i].reserved_size);
    }
    free(stats);

    // Grab the heap statistics
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    printf("Heap size: %lu / %lu bytes\r\n", heap_stats.current_size, heap_stats.reserved_size);

    mbed_stats_cpu_t cpu_stats;
    mbed_stats_cpu_get(&cpu_stats);

    printf("CPU: uptime=%lld, idle=%lld\r\n", cpu_stats.uptime, cpu_stats.idle_time);
    printf("Sleep: sleep=%lld, deepsleep=%lld\r\n", cpu_stats.sleep_time, cpu_stats.deep_sleep_time);
    printf("\r\n");
}

#endif // STATS_REPORT_H

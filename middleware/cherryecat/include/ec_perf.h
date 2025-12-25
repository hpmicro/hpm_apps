/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EC_PERF_H
#define EC_PERF_H

typedef struct {
    bool enable;    // Enable performance measurement
    uint64_t count; // Current measurement count

    uint64_t min_interval;    // Minimum interval
    uint64_t max_interval;    // Maximum interval
    int64_t min_jitter;       // Minimum jitter
    int64_t max_jitter;       // Maximum jitter
    uint64_t total_interval;  // Total interval time
    int64_t total_jitter;     // Total jitter (for average calculation)

    uint32_t ignore_count;      // Number of ignored measurements
    uint64_t last_timestamp;    // Last interrupt timestamp
    uint64_t expected_interval; // Expected interrupt interval
} ec_perf_t;

void ec_perf_init(ec_perf_t *perf, uint64_t expected_interval_us);
void ec_perf_polling(ec_perf_t *perf);
bool ec_perf_is_complete(ec_perf_t *perf);
void ec_perf_print_statistics(ec_perf_t *perf);

#endif
/*
 * Copyright (c) 2025, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ec_master.h"

#ifdef CONFIG_EC_PERF_ENABLE
void ec_perf_init(ec_perf_t *perf, uint64_t expected_interval_us)
{
    memset(perf, 0, sizeof(ec_perf_t));

    perf->enable = true;
    perf->min_interval = UINT64_MAX;
    perf->max_interval = 0;
    perf->min_jitter = INT64_MAX;
    perf->max_jitter = INT64_MIN;
    perf->expected_interval = expected_interval_us;
    perf->ignore_count = 5;

    EC_LOG_RAW("Perf initialized\n");
    EC_LOG_RAW("Expected interval: %llu us\n", expected_interval_us);
}

EC_FAST_CODE_SECTION void ec_perf_polling(ec_perf_t *perf)
{
    uint64_t current_timestamp = jiffies;

    if (!perf->enable) {
        return;
    }

    if (perf->ignore_count > 0) {
        perf->ignore_count--;
        perf->last_timestamp = current_timestamp;
        return;
    }

    uint64_t interval = current_timestamp - perf->last_timestamp;
    int64_t jitter = (int64_t)interval - (int64_t)perf->expected_interval;

    if (interval < perf->min_interval)
        perf->min_interval = interval;
    if (interval > perf->max_interval)
        perf->max_interval = interval;
    if (jitter < perf->min_jitter)
        perf->min_jitter = jitter;
    if (jitter > perf->max_jitter)
        perf->max_jitter = jitter;

    perf->total_interval += interval;
    perf->total_jitter += jitter;

    perf->count++;
    perf->last_timestamp = current_timestamp;
}

void ec_perf_print_statistics(ec_perf_t *perf)
{
    if (perf->count == 0)
        return;

    double avg_interval = (double)perf->total_interval / perf->count;
    double avg_jitter = (double)perf->total_jitter / perf->count;

    EC_LOG_RAW("\n========= Perf Statistics =========\n");
    EC_LOG_RAW("Measurements: %lld\n", perf->count);
    EC_LOG_RAW("\nInterval Statistics (us):\n");
    EC_LOG_RAW("  Average:    %.2f\n", avg_interval);
    EC_LOG_RAW("  Minimum:    %llu\n", perf->min_interval);
    EC_LOG_RAW("  Maximum:    %llu\n", perf->max_interval);

    EC_LOG_RAW("\nJitter Statistics (us):\n");
    EC_LOG_RAW("  Average:        %.2f\n", avg_jitter);
    EC_LOG_RAW("  Minimum:        %lld\n", perf->min_jitter);
    EC_LOG_RAW("  Maximum:        %lld\n", perf->max_jitter);

    EC_LOG_RAW("===================================\n");
}
#endif
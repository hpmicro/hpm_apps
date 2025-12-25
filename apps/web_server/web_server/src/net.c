/*
 * Copyright (c) 2025 HPMicro
 * Copyright (c) 2023 Cesanta Software Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "net.h"
#include "button.h"
#include <stddef.h>

#define NO_CACHE_HEADERS "Cache-Control: no-cache\r\n"
#define JSON_HEADERS     "Content-Type: application/json\r\n" NO_CACHE_HEADERS

struct attribute {
    const char *name;
    const char *type;
    const char *format;
    size_t offset;
    size_t size;
    bool readonly;
};

void mg_json_get_str2(struct mg_str json, const char *path, char *buf, size_t len)
{
    struct mg_str s = mg_json_get_tok(json, path);
    if (s.len > 1 && s.buf[0] == '"') {
        mg_json_unescape(mg_str_n(s.buf + 1, s.len - 2), buf, len);
    }
}

// SNTP timer function. Sync up time
static void timer_sntp_fn(void *param)
{
    mg_sntp_connect(param, "udp://time.google.com:123", NULL, NULL);
}

// HTTP request handler function
static void fn(struct mg_connection *c, int ev, void *ev_data)
{
    if (ev == MG_EV_ACCEPT) {
        if (c->fn_data != NULL) { // TLS listener!
            struct mg_tls_opts opts = { 0 };
            opts.cert = mg_unpacked("/certs/server_cert.pem");
            opts.key = mg_unpacked("/certs/server_key.pem");
            mg_tls_init(c, &opts);
        }
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;

        // Handle /api/heartbeat endpoint - simple alive check
        if (hm->uri.len == sizeof("/api/heartbeat") - 1 && strncmp(hm->uri.buf, "/api/heartbeat", hm->uri.len) == 0) {
            /* Return basic device info to indicate device is alive */
            mg_http_reply(c, 200, JSON_HEADERS, "{%m:%d}\n", MG_ESC("firmwareVersion"), 0x010000); // Version 1.0.0
            return;
        }

        // Handle /api/di endpoint - return digital input states as bit mask
        if (hm->uri.len == sizeof("/api/di") - 1 && strncmp(hm->uri.buf, "/api/di", hm->uri.len) == 0) {
            /* Return persistent di_state directly. Bits 0..5 map to IN1..IN6 */
            uint8_t di_bits = di_state;
            mg_http_reply(c, 200, JSON_HEADERS, "{%m:%d}\n", MG_ESC("digitalInputs"), di_bits);
            return;
        }

        // Handle /api/segment-display endpoint - display digit on segment LED
        if (hm->uri.len == sizeof("/api/segment-display") - 1 && strncmp(hm->uri.buf, "/api/segment-display", hm->uri.len) == 0) {
            /* Parse JSON body to get digit value */
            int digit = -1;
            double digit_double = 0;

            // Extract "digit" field from JSON body as number
            if (mg_json_get_num(hm->body, "$.digit", &digit_double) > 0) {
                digit = (int)digit_double;
            }

            // Validate digit range (0-9)
            if (digit >= 0 && digit <= 9) {
                /* Display digit at position 2 using raw segment data */
                segment_led_set_disp_data(2, (uint16_t)s_disp_code_8_seg[digit]);
                mg_http_reply(c, 200, JSON_HEADERS, "{%m:%s,%m:%d}\n", MG_ESC("status"), "success", MG_ESC("digit"), digit);
            } else {
                mg_http_reply(c, 400, JSON_HEADERS, "{%m:%s,%m:%s}\n", MG_ESC("status"), "error", MG_ESC("message"), "Digit must be 0-9");
            }
            return;
        }
        struct mg_http_serve_opts opts;
        memset(&opts, 0, sizeof(opts));
        opts.root_dir = "/web_root"; // On embedded, use packed files
        opts.fs = &mg_fs_packed;
        mg_http_serve_dir(c, ev_data, &opts);
        MG_DEBUG(("%lu %.*s %.*s", c->id, (int)hm->method.len, hm->method.buf, (int)hm->uri.len, hm->uri.buf));
    }
}

// web init
void web_init(struct mg_mgr *mgr)
{
    mg_http_listen(mgr, HTTP_URL, fn, NULL);
    mg_http_listen(mgr, HTTPS_URL, fn, (void *)1);
    mg_timer_add(mgr, 10 * 60 * 1000, MG_TIMER_RUN_NOW | MG_TIMER_REPEAT, timer_sntp_fn, mgr);
}

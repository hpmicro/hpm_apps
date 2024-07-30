#ifndef DEMO_KEY_H
#define DEMO_KEY_H

typedef enum
{
    DEMO_KEY1 = 0,
    DEMO_KEY2,
    DEMO_KEY3,
    DEMO_KEY4,
    DEMO_KEY_MAX
} DEMO_KEY_INDEX;

typedef void(*demo_key_cb)(uint8_t index, uint8_t level);

uint8_t demo_keys_read_level(uint8_t index);

void demo_keys_init(demo_key_cb cb);

#endif
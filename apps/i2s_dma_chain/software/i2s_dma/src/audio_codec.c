/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "i2s_dma_chain.h"

int main(void)
{
    board_init();
    i2s_dma_tx_init();
    i2s_dma_rx_init();
    printf("i2s dma chain rx and tx example!\n");
    while(1){
      rx_data_process();
    };
    return 0;
}

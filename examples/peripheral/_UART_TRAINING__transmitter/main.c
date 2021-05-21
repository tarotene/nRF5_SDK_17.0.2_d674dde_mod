/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup uart_example_main main.c
 * @{
 * @ingroup uart_example
 * @brief UART Example Application main file.
 *
 * This file contains the source code for a sample application using UART.
 *
 */

#include <inttypes.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

//#define ENABLE_LOOPBACK_TEST  /**< if defined, then this example will be a loopback test, which means that TX should be connected to RX to get data loopback. */

#define MAX_TEST_DATA_BYTES (15U) /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256      /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256      /**< UART RX buffer size. */

void uart_error_handle(app_uart_evt_t *p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}

#ifdef ENABLE_LOOPBACK_TEST
/* Use flow control in loopback test. */
#define UART_HWFC APP_UART_FLOW_CONTROL_ENABLED

/** @brief Function for setting the @ref ERROR_PIN high, and then enter an infinite loop.
 */
static void show_error(void)
{

    bsp_board_leds_on();
    while (true)
    {
        // Do nothing.
    }
}

/** @brief Function for testing UART loop back.
 *  @details Transmitts one character at a time to check if the data received from the loopback is same as the transmitted data.
 *  @note  @ref TX_PIN_NUMBER must be connected to @ref RX_PIN_NUMBER)
 */
static void uart_loopback_test()
{
    uint8_t *tx_data = (uint8_t *)("\r\nLOOPBACK_TEST\r\n");
    uint8_t rx_data;

    // Start sending one byte and see if you get the same
    for (uint32_t i = 0; i < MAX_TEST_DATA_BYTES; i++)
    {
        uint32_t err_code;
        while (app_uart_put(tx_data[i]) != NRF_SUCCESS)
            ;

        nrf_delay_ms(10);
        err_code = app_uart_get(&rx_data);

        if ((rx_data != tx_data[i]) || (err_code != NRF_SUCCESS))
        {
            show_error();
        }
    }
    return;
}
#else
/* When UART is used for communication with the host do not use flow control.*/
#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED
#endif

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t err_code;

    bsp_board_init(BSP_INIT_LEDS);

    const app_uart_comm_params_t comm_params =
    { RX_PIN_NUMBER,
      TX_PIN_NUMBER,
      RTS_PIN_NUMBER,
      CTS_PIN_NUMBER,
      UART_HWFC,
      false,
#if defined(UART_PRESENT)
      NRF_UART_BAUDRATE_115200
#else
      NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);

    APP_ERROR_CHECK(err_code);

#ifndef ENABLE_LOOPBACK_TEST
    printf("\r\nUART example started.\r\n");

    NRF_GPIO_Type *reg = (NRF_GPIO_Type *)0x50000000UL;
    reg->PIN_CNF[13] = (0UL << 0UL) | (0UL << 1UL) | (3UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[14] = (0UL << 0UL) | (0UL << 1UL) | (3UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[15] = (0UL << 0UL) | (0UL << 1UL) | (3UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[16] = (0UL << 0UL) | (0UL << 1UL) | (3UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[17] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[18] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[19] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[20] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);

    bsp_board_init(BSP_INIT_BUTTONS);

    while (true)
    {
        // uint8_t cr;
        // while (app_uart_get(&cr) != NRF_SUCCESS);
        // while (app_uart_put(cr) != NRF_SUCCESS);

        // reg->OUT &= ~(15UL << 17);
        // reg->OUT |= (cr << 17);

        // if (cr == 'q' || cr == 'Q')
        // {
        //     printf(" \r\nExit!\r\n");

        //     while (true)
        //     {
        //         // Do nothing.
        //     }
        // }

        /* 
            ビット番号
            {
            GPIO_PIN_CNF_DIR_Pos     := 0UL
            GPIO_PIN_CNF_INPUT_Pos   := 1UL
            GPIO_PIN_CNF_PULL_Pos    := 2UL
            GPIO_PIN_CNF_DRIVE_Pos   := 8UL
            GPIO_PIN_CNF_SENSE_Pos   := 16UL
            }
            にビット値
            {
            DIR     = INPUT     := 0UL,
            INPUT   = CONNECT   := 1UL,
            PULL    = PULLUP    := 3UL,
            DRIVE   = BOTH STD  := 1UL,
            SENSE   = NOSENSE   := 0UL
            }
            をそれぞれセットしたものをGPIO XX (XX = 13, 14, 15, 16) 番ピンのPIN_CNFレジスタに代入．
        */

        // #define STATE_BUTTON_2 (reg->IN >> 14) & 1UL;
        // #define STATE_BUTTON_3 (reg->IN >> 15) & 1UL;
        // #define STATE_BUTTON_4 (reg->IN >> 16) & 1UL;

        // uint32_t buf_button_1 = STATE_BUTTON_1;
        // uint32_t buf_button_2 = STATE_BUTTON_2;
        // uint32_t buf_button_3 = STATE_BUTTON_3;
        // uint32_t buf_button_4 = STATE_BUTTON_4;

        // while (buf_button_1 == STATE_BUTTON_1 && buf_button_2 == STATE_BUTTON_2 && buf_button_3 == STATE_BUTTON_3 && buf_button_4 == STATE_BUTTON_4)
        // {
        //     /* wait until any button's state changes */
        // }
        uint32_t reg_buttons = ((reg->IN >> 13) & 15UL);

        while (((reg->IN >> 13) & 15UL) == reg_buttons)
        {
            /* wait until any of the buttons' is pushed */
        }
        uint8_t ct = 0x30 + ((reg->IN >> 13) & 15UL);
        // app_uart_put(ct);
        printf("%c", ct);
    }
#else

    // This part of the example is just for testing the loopback .
    while (true)
    {
        uart_loopback_test();
    }
#endif
}

/** @} */
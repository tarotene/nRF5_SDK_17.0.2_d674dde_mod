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
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdint.h>
// #include <stdio.h>
// #include <inttypes.h>
// #include "nrf_delay.h"
// #include "boards.h"

// #include "SEGGER_RTT.h"

typedef struct
{ /*!< (@ 0x50000000) P0 Structure                                               */
    volatile const uint32_t RESERVED[321];
    volatile uint32_t OUT;        /*!< (@ 0x00000504) Write GPIO port                                            */
    volatile uint32_t OUTSET;     /*!< (@ 0x00000508) Set individual bits in GPIO port                           */
    volatile uint32_t OUTCLR;     /*!< (@ 0x0000050C) Clear individual bits in GPIO port                         */
    volatile const uint32_t IN;   /*!< (@ 0x00000510) Read GPIO port                                             */
    volatile uint32_t DIR;        /*!< (@ 0x00000514) Direction of GPIO pins                                     */
    volatile uint32_t DIRSET;     /*!< (@ 0x00000518) DIR set register                                           */
    volatile uint32_t DIRCLR;     /*!< (@ 0x0000051C) DIR clear register                                         */
    volatile uint32_t LATCH;      /*!< (@ 0x00000520) Latch register indicating what GPIO pins that
                                                                    have met the criteria set in the PIN_CNF[n].SENSE
                                                                    registers                                                  */
    volatile uint32_t DETECTMODE; /*!< (@ 0x00000524) Select between default DETECT signal behaviour
                                                                    and LDETECT mode                                           */
    volatile const uint32_t RESERVED1[118];
    volatile uint32_t PIN_CNF[32]; /*!< (@ 0x00000700) Description collection[0]: Configuration of GPIO
                                                                    pins                                                       */
} NRF_GPIO_Type;                   /*!< Size = 1920 (0x780)                                                       */

#define NRF_GPIO_BASE ((NRF_GPIO_Type *)0x50000000UL)

typedef void (*delay_func_t)(uint32_t);
/**
 * @brief Function for application main entry.
 */
int main(void)
{   
    // ループ関数の定義
    static const uint16_t delay_machine_code[] = {
        0x3800 + 3, // SUBS r0, #loop_cycles
        0xd8fd,                                     // BHI .-2
        0x4770                                      // BX LR
    };

    // GPIOのベースアドレスへのポインタを取得
    NRF_GPIO_Type *reg = (NRF_GPIO_Type *)0x50000000UL;

    // GPIOのLEDに対応する4本のピンを設定
    reg->PIN_CNF[17] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[18] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[19] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
    reg->PIN_CNF[20] = (1UL << 0UL) | (1UL << 1UL) | (0UL << 2UL) | (1UL << 8UL) | (0UL << 16UL);
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
     DIR     = OUTPUT       := 1UL,
     INPUT   = DISCONNECT   := 1UL,
     PULL    = NOPULL       := 0UL,
     DRIVE   = BOTH STD     := 1UL,
     SENSE   = NOSENSE      := 0UL
    }
    をそれぞれセットしたものをGPIO XX (XX = 17, 18, 19, 20) 番ピンのPIN_CNFレジスタに代入．
    */

    reg->OUTSET |= ((1UL << 17) | (1UL << 18) | (1UL << 19) | (1UL << 20)); // LED Global Switch-off (GPIO P0.17-P.0.20)

    uint32_t ms_time;

    // typedef void (*delay_func_t)(uint32_t);

    /* Toggle LEDs. */
    while (1)
    {
        reg->OUT ^= (1UL << 17);
        ms_time = 500;
        do
        {
            const delay_func_t delay_cycles = (delay_func_t)((((uint32_t)delay_machine_code) | 1)); // Set LSB to 1 to execute the code in the Thumb mode.
            // const delay_func_t delay_cycles = (delay_func_t)((uint32_t)delay_machine_code);
            delay_cycles((uint32_t)6400);
        } while (--ms_time);

        reg->OUT ^= (1UL << 18);
        ms_time = 500;
        do
        {
            const delay_func_t delay_cycles = (delay_func_t)((((uint32_t)delay_machine_code) | 1)); // Set LSB to 1 to execute the code in the Thumb mode.
            // const delay_func_t delay_cycles = (delay_func_t)((uint32_t)delay_machine_code);
            delay_cycles((uint32_t) 6400);
        } while (--ms_time);

        reg->OUT ^= (1UL << 19);
        ms_time = 500;
        do
        {
            const delay_func_t delay_cycles = (delay_func_t)((((uint32_t)delay_machine_code) | 1)); // Set LSB to 1 to execute the code in the Thumb mode.
            // const delay_func_t delay_cycles = (delay_func_t)((uint32_t)delay_machine_code);
            delay_cycles((uint32_t)6400);
        } while (--ms_time);

        reg->OUT ^= (1UL << 20);
        ms_time = 500;
        do
        {
            const delay_func_t delay_cycles = (delay_func_t)((((uint32_t)delay_machine_code) | 1)); // Set LSB to 1 to execute the code in the Thumb mode.
            // const delay_func_t delay_cycles = (delay_func_t)((uint32_t)delay_machine_code);
            delay_cycles((uint32_t)6400);
        } while (--ms_time);
    }
}

/**
 *@}
 **/

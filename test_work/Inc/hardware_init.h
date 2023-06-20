/*
 * hardware_init.h
 *
 * Created: 2023/6/8 下午 03:55:38
 *  Author: ZongYo
 */ 


#ifndef HARDWARE_INIT_H_
#define HARDWARE_INIT_H_

#include "uartdevice.h"
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#undef ISR
#ifdef __cplusplus
#    define ISR(vector, ...)                                         \
extern "C" void vector##_routine(void) __attribute__((__INTR_ATTRS)) \
__VA_ARGS__;                                                         \
void vector##_routine(void)
#else
#    define ISR(vector, ...)                                                   \
void vector##_routine(void) __attribute__((__INTR_ATTRS)) __VA_ARGS__; \
void vector##_routine(void)
#endif

/**
 * @def REGFPT(REG_P, MASK, SHIFT, DATA)
 * @ingroup macro_macro
 * @brief 依照指定的 MASK, SHIFT, DATA 去讀取暫存器
 */
#define REGFPT(REG_P, MASK, SHIFT, DATA)                                       \
    (*((volatile char *)REG_P) = ((*((volatile char *)REG_P) & (~MASK)) |      \
                                  (((DATA) << (SHIFT)) & (MASK))))

/**
 * @def REGFGT(REG_P, MASK, SHIFT, DATA_P)
 * @ingroup macro_macro
 * @brief 依照指定的 MASK, SHIFT, DATA_P 去寫入暫存器
 */
#define REGFGT(REG_P, MASK, SHIFT, DATA_P)                                     \
    (*((volatile char *)DATA_P) =                                              \
         ((*((volatile char *)REG_P) & (MASK)) >> (SHIFT)))

/**
 * @def REGPUT(REG_P, BYTES, DATA_P)
 * @ingroup macro_macro
 * @brief 依照指定BYTES數的 *DATA_P 去寫入暫存器 *REG_P 。
 */
#define REGPUT(REG_P, BYTES, DATA_P)                                           \
    do {                                                                       \
        for (signed char __putptr = BYTES - 1; __putptr >= 0; __putptr--) {    \
            *((volatile char *)REG_P + __putptr) =                             \
                *((volatile char *)DATA_P + __putptr);                         \
        }                                                                      \
    } while (0)

/**
 * @def REGGET(REG_P, BYTES, DATA_P)
 * @ingroup macro_macro
 * @brief 依照指定BYTES數從暫存器 *REG_P 取得數值存入DATA_P。
 */
#define REGGET(REG_P, BYTES, DATA_P)                                           \
    do {                                                                       \
        for (char __getptr = 0; __getptr < BYTES; __getptr++) {				   \
            *((volatile char *)DATA_P + __getptr) =                            \
                *((volatile char *)REG_P + __getptr);                          \
        }                                                                      \
    } while (0)


#define P0 (0x01U)
#define P1 (0x02U)
#define P2 (0x04U)
#define P3 (0x08U)
#define P4 (0x10U)
#define P5 (0x20U)
#define P6 (0x40U)
#define P7 (0x80U)

void hardware_init();

#endif /* HARDWARE_INIT_H_ */
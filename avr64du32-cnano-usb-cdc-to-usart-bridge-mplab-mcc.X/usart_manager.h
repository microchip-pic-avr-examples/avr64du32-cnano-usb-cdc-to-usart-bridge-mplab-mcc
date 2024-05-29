/**
 * USART USART Header File
 * @file usart.h
 * @defgroup usart
 * @brief Contains the prototypes and data types for the USART implementation
 * @version USART Driver Version 1.0.0
 */

/*
    (c) 2021 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
 */

#ifndef USART_H_
#define USART_H_

#include <xc.h>

#include <stdint.h>
#include <stdbool.h>

#define USART USART1
#define USART_PORT PORTD
#define USART_RX_bm PIN7_bm
#define USART_TX_bm PIN6_bm
#define USART_PORTMUX() (PORTMUX.USARTROUTEA |= PORTMUX_USART1_ALT2_gc) // Default pins

/**
 * @ingroup usart
 * @enum USART_RETURN_CODE_enum
 * @brief Enum describes the different function return reserved codes used by the USART driver.
 */
typedef enum USART_RETURN_CODE_enum
{
    USART_SUCCESS = 0,
    USART_BUFFER_FULL = -1,
    USART_BUFFER_EMPTY = -2
} USART_RETURN_CODE_t;

/**
 * @ingroup usart
 * @brief Handler for TX/RX that will send or receive data if available in buffer or register
 * @return usartStatus - Handler status after both RX and TX communication attempt
 */
USART_RETURN_CODE_t USART_Handler(void);

/**
 * @ingroup usart
 * @brief Checks if data available in USART receive buffer
 * @retval 0 - Buffer is empty
 * @retval 1 - Data available
 */
bool USART_RxBufferReceived(void);

/**
 * @ingroup usart
 * @brief Checks if the transmit buffer is full to avoid overwriting or losing data
 * @retval 0 - Buffer not full
 * @retval 1 - Buffer full
 */
bool USART_TxBufferFull(void);

/**
 * @ingroup usart
 * @brief Adds data to USART transmit buffer if buffer not full
 * @param data - Data to be added
 * @return status - Success or buffer full depending on buffer state
 */
USART_RETURN_CODE_t USART_TxBufferWrite(uint8_t data);

/**
 * @ingroup usart
 * @brief Retrieves data from USART receive buffer if buffer not empty
 * @param data - Pointer to data byte to store buffer data
 * @return status - Success or buffer empty depending on buffer state
 */
USART_RETURN_CODE_t USART_RxBufferRead(uint8_t *data);

#endif /* USART_H_ */
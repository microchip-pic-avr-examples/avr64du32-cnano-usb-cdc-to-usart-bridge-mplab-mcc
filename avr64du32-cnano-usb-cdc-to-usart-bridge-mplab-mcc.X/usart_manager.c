/**
 * USART USART Source File
 * @file usart.c
 * @defgroup usart
 * @brief Contains the prototypes and data types for the USART implementation
 * @version USART Driver Version 1.0.0
 * @{
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

#include "usart_manager.h"

#include <string.h>

#include <usb_config.h>
#include <circular_buffer.h>
#include "usart1.h"

#define USART_BAUD(BAUD) ((float)(F_CPU * 64 / (16 * (float)BAUD)) + 0.5)

#define USART_TX_BUFFER_SIZE USB_CDC_RX_BUFFER_SIZE
#define USART_RX_BUFFER_SIZE USB_CDC_TX_BUFFER_SIZE

// Buffers
// RX
static uint8_t usartReceiveArray[USART_RX_BUFFER_SIZE];
static CIRCULAR_BUFFER_t usartReceiveBuffer = {
    .content = usartReceiveArray,
    .head = 0,
    .tail = 0,
    .maxLength = USART_RX_BUFFER_SIZE,
};

// TX
static uint8_t usartTransmitArray[USART_TX_BUFFER_SIZE];
static CIRCULAR_BUFFER_t usartTransmitBuffer = {
    .content = usartTransmitArray,
    .head = 0,
    .tail = 0,
    .maxLength = USART_TX_BUFFER_SIZE,
};

USART_RETURN_CODE_t USART_Handler(void)
{
    BUFFER_RETURN_CODE_t bufferStatus = BUFFER_SUCCESS;
    USART_RETURN_CODE_t usartStatus = USART_SUCCESS;
    uint8_t data;

    if (true == USART1_IsTxReady()) // Checks if USART ready to transmit
    {
        // Checks if data available in buffer
        bufferStatus = CIRCBUF_Dequeue(&usartTransmitBuffer, &data);
        if (BUFFER_SUCCESS == bufferStatus)
        {
            USART1_Write(data);
        }
        else
        {
            ; // TX buffer empty, no data to transmit
        }
    }
    else
    {
        ; // USART TX not ready, transmission in progress
    }

    if (false == CIRCBUF_Full(&usartReceiveBuffer)) // Checks if RX buffer can receive
    {
        while (USART1_IsRxReady()) // Receives data as long as there is data in the buffer
        {
            data = USART1_Read();
            bufferStatus = CIRCBUF_Enqueue(&usartReceiveBuffer, data);

            // Checks if buffer now is full
            if (BUFFER_FULL == bufferStatus)
            {
                usartStatus = USART_BUFFER_FULL;
                break; // Exits loop
            }
        }
    }
    else
    {
        usartStatus = USART_BUFFER_FULL; // RX buffer full return error
    }

    return usartStatus;
}

bool USART_RxBufferReceived(void)
{
    return !CIRCBUF_Empty(&usartReceiveBuffer);
}

bool USART_TxBufferFull(void)
{
    return CIRCBUF_Full(&usartTransmitBuffer);
}

USART_RETURN_CODE_t USART_TxBufferWrite(uint8_t data)
{
    return CIRCBUF_Enqueue(&usartTransmitBuffer, data);
}

USART_RETURN_CODE_t USART_RxBufferRead(uint8_t *data)
{
    return CIRCBUF_Dequeue(&usartReceiveBuffer, data);
}
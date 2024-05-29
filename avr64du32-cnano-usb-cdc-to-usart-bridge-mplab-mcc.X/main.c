/**
 * MAIN
 * @file main.c
 * @defgroup main MAIN
 * @brief This file contain an example for running a USB CDC to USART bridge for AVR DU
 * @version MAIN Driver Version 1.0.0
 */

/*
� [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip
    software and any derivatives exclusively with Microchip products.
    You are responsible for complying with 3rd party license terms
    applicable to your use of 3rd party software (including open source
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.?
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.
*/

#include "mcc_generated_files/system/system.h"
#include "usb_cdc.h"
#include "usb_cdc_virtual_serial_port.h"
#include "usart_manager.h"

// Status variables
volatile RETURN_CODE_t status = SUCCESS;
volatile CDC_RETURN_CODE_t cdcStatus = CDC_SUCCESS;
volatile USART_RETURN_CODE_t usartStatus = USART_SUCCESS;

// Data variables
uint8_t usartData;
uint8_t cdcData;

int main(void)
{
    SYSTEM_Initialize();

    while (1)
    {
        // Running USB handle
        status = USBDevice_Handle();

        if (SUCCESS == status)
        {
            // Tests if DTE is set and USB ready for transfer
            if (true == USB_CDCDataTerminalReady())
            {
                // Checks that CDC TX buffer is not full or pipe is busy
                if (false == USB_CDCTxBusy())
                {
                    // Retrieves USART data if available and writes to CDC
                    usartStatus = USART_RxBufferRead(&usartData);
                    if (USART_SUCCESS == usartStatus)
                    {
                        cdcStatus = USB_CDCWrite(usartData);
                    }
                    else
                    {
                        usartStatus = USART_SUCCESS; // No USART data available
                    }
                }
                else
                {
                    ; // CDC TX buffer full, retry on next iteration
                }

                // Checks that USART buffer is not full
                if (false == USART_TxBufferFull())
                {
                    // Retrieves CDC data if available and writes to USART
                    cdcStatus = USB_CDCRead(&cdcData);
                    if (CDC_SUCCESS == cdcStatus)
                    {
                        usartStatus = USART_TxBufferWrite(cdcData);
                    }
                    else
                    {
                        cdcStatus = CDC_SUCCESS; // No CDC data available
                    }
                }
                else
                {
                    ; // USART TX buffer full, retry on next iteration
                }

                // Running CDC and USART handlers
                status = USB_CDCVirtualSerialPortHandler();
                if (SUCCESS == status && USART_SUCCESS == usartStatus)
                {
                    usartStatus = USART_Handler();
                }
                else
                {
                    ; // CDC Handler or previous USART Write failed, skipping USART Handler
                }
            }
            else
            {
                ; // No terminal session active
            }
        }
        else
        {
            ; // USB handle error
        }
    }
}
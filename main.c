/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the PSOC Control C1 MCU: CCU8 Timer synchronous
*              start Example for ModusToolbox.
*
* Related Document: See README.md
*
*******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include <stdio.h>
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/*Define macros for KIT_PSC1M3_EVK kit*/
#define INTERRUPT_PRIORITY_NODE_ID              IRQ3_IRQn
#define INTERRUPT_EVENT_PRIORITY                (3U)
#define ERU_EXTERNAL_EVENT_HANDLER              IRQ_Hdlr_3
#define TIMER_0_PERIOD_MATCH_EVENT_PRIORITY     3
#define TIMER_1_PERIOD_MATCH_EVENT_PRIORITY     3
#define TIMER_0_PERIOD_MATCH_EVENT_HANDLER      CCU80_SR0_INTERRUPT_HANDLER
#define TIMER_0_PERIOD_MATCH_EVENT_IRQN         CCU80_SR0_IRQN
#define TIMER_1_PERIOD_MATCH_EVENT_HANDLER      CCU81_SR0_INTERRUPT_HANDLER
#define TIMER_1_PERIOD_MATCH_EVENT_IRQN         CCU81_SR0_IRQN

/* Define macro to enable/disable printing of debug messages */
#define ENABLE_DEBUG_PRINT                      (0)

/* Define macro to check if loop is entered once */
#if ENABLE_DEBUG_PRINT
static bool ENTER_LOOP1 = false;
static bool ENTER_LOOP2 = false;
#endif

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Flags set in timer interrupts and checked in the main loop */
static volatile bool timer0_interrupt_flag = false;
static volatile bool timer1_interrupt_flag = false;

/*******************************************************************************
* Function Name: TIMER_0_PERIOD_MATCH_EVENT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the CCU8 timer period match interrupt.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void TIMER_0_PERIOD_MATCH_EVENT_HANDLER(void)
{
    timer0_interrupt_flag = true;
}
/*******************************************************************************
* Function Name: TIMER_1_PERIOD_MATCH_EVENT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the CCU81 timer period match interrupt.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void TIMER_1_PERIOD_MATCH_EVENT_HANDLER(void)
{
    timer1_interrupt_flag = true;
}

/*******************************************************************************
* Function Name: configure_eru
********************************************************************************
* Summary:
* This is the function which initializes all the parameters needed for the ERU.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void configure_eru(void)
{
    /*Set Priority for IRQ*/
    NVIC_SetPriority(INTERRUPT_PRIORITY_NODE_ID, INTERRUPT_EVENT_PRIORITY);

    /*Enable the Interrupt*/
    NVIC_EnableIRQ(INTERRUPT_PRIORITY_NODE_ID);
}

/*******************************************************************************
* Function Name: configure_timer0_interrupt
********************************************************************************
* Summary:
* This is the function which configures the parameters needed for the interrupt that
* will be generated by the period match of the CCU80 timer.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void configure_timer0_interrupt(void)
{

    NVIC_SetPriority(TIMER_0_PERIOD_MATCH_EVENT_IRQN, TIMER_0_PERIOD_MATCH_EVENT_PRIORITY);

    /*Enable IRQ*/
    NVIC_EnableIRQ(TIMER_0_PERIOD_MATCH_EVENT_IRQN);
}

/*******************************************************************************
* Function Name: configure_timer1_interrupt
********************************************************************************
* Summary:
* This is the function which configures the parameters needed for the interrupt that
* will be generated by the period match of the CCU81 timer.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void configure_timer1_interrupt(void)
{

    NVIC_SetPriority(TIMER_1_PERIOD_MATCH_EVENT_IRQN, TIMER_1_PERIOD_MATCH_EVENT_PRIORITY);

    /*Enable IRQ*/
    NVIC_EnableIRQ(TIMER_1_PERIOD_MATCH_EVENT_IRQN);
}

/*******************************************************************************
* Function Name: ERU_EXTERNAL_EVENT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the ERU external interrupt which starts
* the CCU8 timers and configures their respective interrupts for the toggling of LED's
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void ERU_EXTERNAL_EVENT_HANDLER(void)
{
    /*Configures the timers to receive the interrupts*/
    configure_timer0_interrupt();
    configure_timer1_interrupt();
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.It sets up a CCU8 timers to period match interrupt.
* The main while loop checks for the elapsed time due to CCU8 timers period match
* event and toggles 2 LEDs at approximately 0.5Hz.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/

int main(void)
{

    cy_rslt_t result;

    /*Initialize the device and board peripherals*/
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /*The USER LEDs on the KIT_PSC1M3_EVK kit are active low. The User LED2 is turned on here */
    Cy_GPIO_SetOutputLow(CYBSP_USER_LED2_PORT, CYBSP_USER_LED2_PIN);

    /*Serial communication initialization function*/
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_HW);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    #if ENABLE_DEBUG_PRINT
    printf("Configuration initialization complete\r\n");
    #endif

    /*Configures the Event Register Unit*/
    configure_eru();

    #if ENABLE_DEBUG_PRINT
    printf("ERU Configured\r\n");
    #endif

    /*Infinite loop*/
    while (1)
    {
        /*Check if timer elapsed (interrupt fired) and toggle the LED*/
        if (timer0_interrupt_flag)
        {
            /*Clear the timer 0 interrupt flag */
            timer0_interrupt_flag = false;
            /* Toggle the USER LED state */
            Cy_GPIO_ToggleOutput(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);
            #if ENABLE_DEBUG_PRINT
            if (!ENTER_LOOP1)
            {
                printf("LED 1 Toggled\r\n");
                ENTER_LOOP1 = true;
            }
            #endif
        }
        if (timer1_interrupt_flag)
        {
            /*Clear the timer 1 interrupt flag */
            timer1_interrupt_flag = false;
            /* Toggle the USER LED state */
            Cy_GPIO_ToggleOutput(CYBSP_USER_LED2_PORT, CYBSP_USER_LED2_PIN);
            #if ENABLE_DEBUG_PRINT
            if (!ENTER_LOOP2)
            {
                printf("LED 2 Toggled\r\n");
                ENTER_LOOP2 = true;
            }
            #endif
        }
    }
}

/* [] END OF FILE */

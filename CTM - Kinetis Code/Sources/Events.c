/* ###################################################################
**     Filename    : Events.c
**     Project     : Lab2
**     Processor   : MK20DN128VLH5
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-08-04, 12:23, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MK20DX128EX5]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnError (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when a channel error (not the error
**         returned by a given method) occurs. The errors can be read
**         using <GetError> method.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_OnError(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnRxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/



void AS1_OnRxChar(void)
{
  /* Write your code here ... */




}

/*
** ===================================================================
**     Event       :  AS1_OnTxChar (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after a character is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_OnTxChar(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnFullRxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called when the input buffer is full;
**         i.e. after reception of the last character 
**         that was successfully placed into input buffer.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AS1_OnFullRxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  AS1_OnFreeTxBuf (module Events)
**
**     Component   :  AS1 [AsynchroSerial]
**     Description :
**         This event is called after the last character in output
**         buffer is transmitted.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void AS1_OnFreeTxBuf(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  Inhr1_OnRxChar (module Events)
**
**     Component   :  Inhr1 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the <Receiver>
**         property is enabled or the <SCI output mode> property (if
**         supported) is set to Single-wire mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
//////////////////////////////////CHARACTER OVER INTERRUPT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
extern volatile char buffer[100];
extern volatile int index;
extern volatile bool command_recieved;
extern volatile bool command_sent;
extern volatile bool hold;
extern volatile bool timer1_interrupted;
//On interrupt
void Inhr1_OnRxChar(void)
{
	TI1_DisableEvent();
	timer1_interrupted = 0;
	Term1_MoveTo(index+1,17);
	hold = 1;

	if (command_recieved) // Will not return until command received == 0
		return; 		// Ignore until processing is complete


	byte recieved_char;
	if (ERR_OK == Inhr1_RecvChar(&recieved_char)) {// Reads Char

		switch (recieved_char) {

			// Backspace
		case 0x7f:
			if (index > 0) {
				index--;				// Decreases Index
				Term1_SendChar(0x7f); 	// Sends Backspace
			}
			break;

			// Enter Key
		case '\r':
			if (index > 0) {
				buffer[index] = '\0'; 			// Insert a trailing null
				index = 0;						// Reset buffer
				command_recieved = 1;			// Command is entered and ready to interpret
			}
			break;

			// All other Keys
		default:
			//Store String
			if (index < 98) { 					// Allow for NULL
				buffer[index] = recieved_char; 	// Save the character
				index++; 						// Increment the index
				Term1_SendChar(recieved_char); 	// Echo to screen

				//Overflow
			} else {
				buffer[1] = 0; 					// Reset buffer
				index = 0;						// Reset index
				Term1_SendStr("\r\n Error Max String Size \r\n");
				command_recieved = 1;
			}//if3
		}//switch1
	}//if-command
}//void-event
//////////////////////////////////CHARACTER OVER INTERRUPT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
/*
** ===================================================================
**     Event       :  ADC_OnEnd (module Events)
**
**     Component   :  ADC [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void ADC_OnEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  ADC_OnCalibrationEnd (module Events)
**
**     Component   :  ADC [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void ADC_OnCalibrationEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     Component   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

//////////////////////////////////TIMERS OVER INTERRUPT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

//Variable for flag
extern volatile bool timer1_interrupted;

void TI1_OnInterrupt(void)
{
	TI1_DisableEvent();//disable events until need, so as to not disrupt the program
	//it was getting stuck if another event was tirggered! :)
	timer1_interrupted = 1;//timer1 flag

}

/*
** ===================================================================
**     Event       :  TI2_OnInterrupt (module Events)
**
**     Component   :  TI2 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

//Variable for flag
extern volatile bool timer2_interrupted;

void TI2_OnInterrupt(void)
{
	timer2_interrupted = 1; //timer2 flag

}
//////////////////////////////////END TIMERS OVER INTERRUPT\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/


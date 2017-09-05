/* ###################################################################
**     Filename    : main.c
**     Project     : Lab2
**     Processor   : MK20DN128VLH5
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-08-04, 12:23, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TU1.h"
#include "Term1.h"
#include "Inhr1.h"
#include "ASerialLdd1.h"
#include "Timer.h"
#include "RealTimeLdd1.h"
#include "TU2.h"
#include "AS1.h"
#include "ASerialLdd2.h"
#include "CsIO1.h"
#include "IO1.h"
#include "I2C.h"
#include "IntI2cLdd1.h"
#include "RED.h"
#include "PwmLdd1.h"
#include "ADC.h"
#include "AdcLdd1.h"
#include "BLUE.h"
#include "PwmLdd2.h"
#include "GREEN.h"
#include "PwmLdd4.h"
#include "TEST_VAL.h"
#include "PwmLdd3.h"
#include "DA1.h"
#include "DacLdd1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

////////////////////////////////// User Includes \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

////////////////////////////////// Variables for Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
	// I2C Variables
	short int xyz_holder[3] = {0,0,0};
	char initialise[2];
	short int x = 0;
	short int y = 0;
	short int z = 0;
	word number_bytes = 0;

	// Max and Min variables
	short int min = 0;
	short int max = 0;

	// Scaled variables
	short int x_scaled = 0;
	short int y_scaled = 0;
	short int z_scaled = 0;


	uint16 wait_time = 0.5;			// Pause for 0.5s (500ms)
	uint16 actual_time = 0;				// Declare uint16 to count up to 65535ms
	uint8 temp_input;

	// variables for events on other page
	volatile char buffer[100];
	volatile char buffer2[100];
	volatile int index;
	volatile bool command_recieved = 0;
	volatile bool command_sent = 0;
	volatile bool hold = 0;

	// Create variables for CT ADC values
	uint16 CT_raw_values[4];
	uint16 CT_1;
	uint16 CT_2;
	uint16 CT_3;
	uint16 CT_4;

	////////////////////////////////// Functions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

	// Command reaction for coding for input to putty
	void CommandReact(){
	if (0 == strcmp(buffer, "stop")) {
		Term1_MoveTo(1,16);Term1_EraseLine();
		hold = 1;
	} else if  (0 == strcmp(buffer,"go")){
		hold = 0;
		Term1_MoveTo(1,16);Term1_EraseLine();
	} else if (sscanf((char *)buffer, "red %hu", &temp_input)){
	} else {
		Term1_MoveTo(1,16);Term1_SendStr("Doesn't make sense bra");
		hold = 0;	}	//endif
	command_sent = 0;   //Reset flag
					}//end commandreact

	//Delay function although not used
	  void Delay(){
	  	for (int i = 0; i <2000000; i++){}
	  }

	  // Measure CT Channel Raw Data

	  void Measure_CT_Channels(){
			ADC_Measure(TRUE);
			ADC_GetValue16(&CT_raw_values);
			CT_1 = CT_raw_values[3]; // pin 1
			CT_2 = CT_raw_values[2]; // pin 2
			CT_3 = CT_raw_values[0]; // pin 3
			CT_4 = CT_raw_values[1]; // pin 4
	  }

	////////////////////////////////// Main User Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
	/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  ////////////////////////////////// One Shot Operations \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
	//start up check and message
	Term1_Cls();	Term1_MoveTo(10,20);
	Term1_SendStr("Warming the Budya Upppp");Term1_CursorDown(1);
	Delay();

	// Initialize the Accelerometer over the I2C Connection
	initialise[0] = 0x2A;														// Initialze F_mode
	initialise[1] = 0x1;														// Active bit as per manual
	I2C_SendBlock(initialise,2,&number_bytes)		;							// Active Accelerometer over I2C

	//error check start up accelerometer
	if (ERR_OK != I2C_SendBlock(initialise,2,&number_bytes)) {
		Term1_SendStr("Your Accelerometer is fingered!!");
		return 0;
	}

	// Create nice terminal window
	Term1_Cls();																	// Clear Terminal
	Term1_MoveTo(3,2);		Term1_SendStr("THE ACCELEROMETER --- 3501");			// Create Title
	Term1_MoveTo(1,4);		Term1_SendStr("X Plane =");
	Term1_MoveTo(1,5);		Term1_SendStr("Y Plane =");	Term1_MoveTo(25,5);		Term1_SendStr("Run = 'go'");
	Term1_MoveTo(1,6);		Term1_SendStr("Z Plane =");	Term1_MoveTo(25,6);		Term1_SendStr("Pause = 'stop'");
	Term1_MoveTo(1,8);		Term1_SendStr("Max (x) =");
	Term1_MoveTo(1,9);		Term1_SendStr("Min (x) =");

	// Reset Timer
	Timer_Reset();

	// Calibare the ADC
	ADC_Calibrate(1);
	  ////////////////////////////////// Primary FOR Loop \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/
	for (;;) { //for-loop-1

  	  // Create delay
		if (ERR_OK == Timer_GetTimeSec(&actual_time)) { //if-1
			}

			// If enough time has passed, update the terminal window
			if ((actual_time > wait_time) & (hold == 0)) {//if-2

							// Reset timer for updating the command window
								if (ERR_OK != Timer_Reset()) {;
								Term1_MoveTo(20,8);Term1_EraseLine();
								Term1_MoveTo(20,8);Term1_SendStr("Not Resetting");
								}
							// error check sending
								if (ERR_OK != I2C_SendChar(0x01)) {
									Term1_MoveTo(20,20);Term1_SendStr("Poxy I2C thing didn't work");
								}
							// error check reading
								if (ERR_OK != I2C_RecvBlock(xyz_holder,6,&number_bytes)) {
									Term1_MoveTo(20,20);Term1_SendStr("Reading error bra!");
								}

							// Extract the x, y and z values from the buffer
							x = xyz_holder[0];
							y = xyz_holder[1];
							z = xyz_holder[2];

							// Print to terminal window
							Term1_MoveTo(12,4);		Term1_SendNum(x);	Term1_SendStr("   ");
							Term1_MoveTo(12,5);		Term1_SendNum(y);	Term1_SendStr("   ");
							Term1_MoveTo(12,6);		Term1_SendNum(z);	Term1_SendStr("   ");

							// Calculate the min and max for the x-plane
							if (x > max) { // if-1
								max = x;
								Term1_MoveTo(12,8);		Term1_SendNum(max);	Term1_SendStr("  ");
							 } else if (x < min) {
								min = x;
								Term1_MoveTo(12,9);		Term1_SendNum(min);	Term1_SendStr("  ");
							} //end-if-1

							// Scale PWN and set
							x_scaled = abs(x)*2;	RED_SetRatio16(x_scaled);
							y_scaled = abs(y)*2;	GREEN_SetRatio16(y_scaled);
							z_scaled = abs(z)*2;	BLUE_SetRatio16(z_scaled);

				// check to see if message is being sent
				snprintf(buffer, 100, "%i,%i,%i\n", xyz_holder[0], xyz_holder[1], xyz_holder[2]);
				word sent;
				//send to zigbee device of asynchro
						if (ERR_OK == AS1_SendBlock(buffer, strlen(buffer), &sent)) {
						Term1_MoveTo(15,12);Term1_SendStr("Data Sent");
						// number of messages sent display sending
						int i; i++;
							Term1_CursorRight(i*3);Term1_SendNum(i);
									if (i == 6){
										Term1_MoveTo(15,12);Term1_EraseLine();
										i = 0;
								}

					//another way to send the data piece-wise
						unsigned int count =0;
						Term1_MoveTo(15+count,13);
						byte out;
						for (count = 0; count < strlen(buffer); count++) 	{
								do {
									out = AS1_SendChar(buffer[count]);
									Term1_SendChar(buffer[count]);
									} while (out != ERR_OK);
													}
				}// if-2

					Measure_CT_Channels();
						Term1_MoveTo(12,15);		Term1_SendNum(CT_1);	Term1_SendStr("       ");
						Term1_MoveTo(12,16);		Term1_SendNum(CT_2);	Term1_SendStr("       ");
						Term1_MoveTo(12,17);Term1_SendStr("CT Current:");		Term1_SendNum((CT_3*3/65536));	Term1_SendStr("       ");
						Term1_MoveTo(12,18);		Term1_SendNum(CT_4);	Term1_SendStr("       ");
						TEST_VAL_SetRatio16(CT_1);


		}// if-1

					//If command is recieved react to it for  typing
						if ((command_recieved == 1) & (command_sent == 0)) {//Recieve Flag
						CommandReact();
						Timer_Reset();
						command_sent = 0;
						command_recieved = 0;
					}//if-flag


  } //end-for-loop-1
  ////////////////////////////////// End Main User Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/
/* END main */
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
*/

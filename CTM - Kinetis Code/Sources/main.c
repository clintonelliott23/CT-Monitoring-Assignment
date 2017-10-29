/* ###################################################################
;**     Filename    : main.c
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
#include "TU2.h"
#include "AS1.h"
#include "ASerialLdd2.h"
#include "CsIO1.h"
#include "IO1.h"
#include "I2C.h"
#include "IntI2cLdd1.h"
#include "ADC.h"
#include "AdcLdd1.h"
#include "CT1_BIT.h"
#include "MUXM.h"
#include "BitIoLdd2.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TI2.h"
#include "TimerIntLdd2.h"
#include "CT1_BIT.h"
#include "BitIoLdd3.h"
#include "SLP.h"
#include "BitIoLdd4.h"
#include "CT2_BIT.h"
#include "BitIoLdd6.h"
#include "CT3_BIT.h"
#include "BitIoLdd5.h"
#include "CT4_BIT.h"
#include "BitIoLdd7.h"
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
	// Variables to be used over putty to recieve a message
	volatile char buffer[100];
	volatile char buffer2[100];
	volatile int index;
	volatile bool command_recieved = 0;
	volatile bool command_sent = 0;
	volatile bool hold = 0;

	// Variables which may be altered
	#define number_samples 250	// Number of samples from the ADC / ms
	float min_val = 0.01; 		// Min value for the channels to DEACTIVATE
	short int drop_out = 3; 	// Amount of cycles the timer is DEACTIVATED

	// Variables to receive data from the ADC and number of samples
	volatile int sample_index;
	unsigned short CT_raw_values[4];
	unsigned short CT1_Raw[number_samples];
	unsigned short CT2_Raw[number_samples];
	unsigned short CT3_Raw[number_samples];
	unsigned short CT4_Raw[number_samples];
	uint16 ADC_measure;

	// Variables for RMS Current
	float CT_Current [4];
	/*float CT2_Current = 0;
	float CT3_Current = 0;
	float CT4_Current = 0;*/

	// Variables for Centre
	float CT1_Centre = 32767;
	float CT2_Centre = 32767;
	float CT3_Centre = 32767;
	float CT4_Centre = 32767;

	//Variables for Min and Max
	float max;			float min;
	float CT1_max;		float CT1_min;
	float CT2_max;		float CT2_min;
	float CT3_max;		float CT3_min;
	float CT4_max;		float CT4_min;
	volatile bool gain_adjust = 0;
	float range = 0;


	//Variables to flag interrupts
	volatile bool timer1_interrupted = 0;
	volatile bool timer2_interrupted = 0;

	// Variable to make channels active with first activated
	volatile bool ch1_measure = 1;
	volatile bool ch2_measure = 0;
	volatile bool ch3_measure = 0;
	volatile bool ch4_measure = 0;

	// Variables to disable the CT channels
	short int CT_Counter = 1;
	uint16 min_input;
	short int disable1 = 0;
	short int disable2 = 0;
	short int disable3 = 0;
	short int disable4 = 0;
	short int reable_counter1 = 0;
	short int reable_counter2 = 0;
	short int reable_counter3 = 0;
	short int reable_counter4 = 0;

	// RMS function variables
	float CT_RMS;
	float centre = 0;
	signed short CT_shifted = 0;

	// Create values for the columns and rows in GUI
	int c1 = 9; int r1 = 7;
	int c11 = 22;
	int c2 = 35; int r2 = 9;
	int c3 = 48; int r3 = 11;
	int c4 = 60; int r4 = 13;


//////////////////////////////////// Functions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

	//Delay function
	  void Delay(){
	  	for (int i = 0; i <2000000; i++){}
	  }

	  void drawGUI(void) {// Function: drawing the initial GUI
		 //start up check and message
		 Term1_Cls();	Term1_MoveTo(10,10);
		 Term1_SendStr("I Current Wait.........!!!");Term1_CursorDown(1);
		 Delay();
		// Create nice terminal window
		 Term1_Cls();	// Clear Terminal
		// Draw title top left
	  	Term1_MoveTo(c11+2, 1);	Term1_SetColor(clCyan, clBlack);
	  	Term1_SendStr("GUI For CT Meters");
	  	// Set boarder colour
	  	Term1_SetColor(clBlack, clCyan);
	  	// Draw crane settings title
	  	Term1_MoveTo(1, 3);
	  	Term1_SendStr("              +-----[ THE CURRENT METER --- 3501 ]-----+            ");
	  	// Draw two border columns
	  	for (int i = 4; i <= 16; i++) {
	  		// Draw left column of first box
	  		Term1_MoveTo(1, i);		Term1_SendStr(" ");
	  		// Draw right column of first box
	  		Term1_MoveTo(68, i);		Term1_SendStr(" ");
	  	}
	  	// Draw bottom Row
	  	for (int i = 1; i <= 68; i++) {
	  		Term1_MoveTo(i, 16);		Term1_SendStr(" ");
	  	}
	  	// Write all information and categories
		Term1_SetColor(clWhite, clBlack);
		Term1_MoveTo(c1, 18);	Term1_SendStr("Type The Following Commands:");
		Term1_MoveTo(c2, 19);	Term1_SendStr("> run    turn on all channels");
		Term1_MoveTo(c2, 20);	Term1_SendStr("> stop   turn off all channels");
		Term1_MoveTo(c2, 21);	Term1_SendStr("> min    new minimum value (amps/1000)");
		Term1_MoveTo(47,17);	Term1_SendStr("Running");
		Term1_MoveTo(c1+3,5);		Term1_SendStr("MAX");
		Term1_MoveTo(c11+3,5);	Term1_SendStr("MIN");
		Term1_MoveTo(c2+3,5);		Term1_SendStr("AVG");
		Term1_MoveTo(c3+1,5);		Term1_SendStr("(%)");
		Term1_MoveTo(c4-2,5);		Term1_SendStr("STATUS");
		Term1_MoveTo(3,r1);		Term1_SendStr("CT1:");
		Term1_MoveTo(3,r2);		Term1_SendStr("CT2:");
		Term1_MoveTo(3,r3);		Term1_SendStr("CT3:");
		Term1_MoveTo(3,r4);		Term1_SendStr("CT4:");
		Term1_MoveTo(c4,r1);	Term1_SendStr("ON");
		Term1_MoveTo(c4,r2);	Term1_SendStr("ON");
		Term1_MoveTo(c4,r3);	Term1_SendStr("ON");
		Term1_MoveTo(c4,r4);	Term1_SendStr("ON");
	  }

		// Command reaction for coding for input to putty
		void CommandReact(){
			// Calculates if the input is valid and discerns outputs
		if (0 == strcmp(buffer, "stop")) {
			Term1_MoveTo(1,17);		Term1_EraseLine();
			Term1_MoveTo(47,17);	Term1_SendStr("STOPPED!");Term1_MoveTo(1,17);
			CT1_BIT_PutVal(0);
			CT2_BIT_PutVal(0);
			CT3_BIT_PutVal(0);
			CT4_BIT_PutVal(0);
			MUXM_PutVal(0);
			hold = 1;
		} else if  (0 == strcmp(buffer,"run")){
			Term1_MoveTo(1,17);		Term1_EraseLine();
			Term1_MoveTo(47,17);	Term1_SendStr("RUNNING...");Term1_MoveTo(1,17);
			hold = 0;
		} else if (sscanf((char *)buffer, "min %hu", &min_input)){
			min_val = (float)(min_input);
			min_val = min_val/1000;
			Term1_MoveTo(1,17);		Term1_EraseLine();
			Term1_MoveTo(47,17);	Term1_SendStr("Min Val = ");Term1_SendFloatNum(min_val);Term1_MoveTo(1,17);
		} else {
			Term1_MoveTo(1,17);		Term1_EraseLine();
			Term1_MoveTo(47,17);	Term1_SendStr("Doesn't make sense bra!");Term1_MoveTo(1,17);
			Delay();
			Term1_MoveTo(1,17);		Term1_EraseLine();
			hold = 0;	}	//endif
		command_sent = 0;   //Reset flag
		TI1_EnableEvent();
		}//end command react

	  // RMS Function
	  float RMS_calculator(unsigned short *CT_data){
		// Reset variables
		  	centre = 0;
			range = 0;
			int avg_total = 0;
			int squared = 0;
			// find the centre using averages
			for (int i = 0; i < number_samples; i++) {
						centre = centre + CT_data[i];
						}//end-for

						// set centre point for min and max and calculate centre
						centre = centre/number_samples;
						max = centre;
						min = centre;

						//Calculates the RMS using squared, sum and square-root
							for (int i = 0; i < number_samples; i++) {
										CT_shifted	= CT_data[i]-centre;
										squared = (CT_shifted*CT_shifted)/number_samples;
										avg_total = avg_total + squared;

										// Find the min and max of the adc channel
										if (CT_data[i]< min){
											min = (float)CT_data[i];
										} else if (CT_data[i]> max){
											max = (float)CT_data[i];
										}
							}//end-for

							//Determine if Gain should be adjusted
								range = max - min;
								if ( range > (65500)){
									gain_adjust = 1;
								} else if ((max > 65534)||(min < 1)){
									gain_adjust = 1;
								} else {
									gain_adjust = 0;
									}

						//	CT_RMS = (((sqrt((float)range)*100)/65535)*30.656);
							CT_RMS = (((((float)range*100)/65530))*0.164908*(6.55/7.86));
							return CT_RMS;
		}//end-function


	  // updates all the values for putty so it can be removed to testing
	void Update_Putty_CT_Values(){
		//CT1 Values
		Term1_MoveTo(c1,r1);		Term1_SendFloatNum(CT1_max);
		Term1_MoveTo(c11,r1);		Term1_SendFloatNum(CT1_min);
		Term1_MoveTo(c2,r1);		Term1_SendFloatNum(CT1_Centre);
		Term1_MoveTo(c3,r1);		Term1_SendFloatNum(CT_Current[0]);
		//CT2 Values
		Term1_MoveTo(c1,r2);		Term1_SendFloatNum(CT2_max);
		Term1_MoveTo(c11,r2);		Term1_SendFloatNum(CT2_min);
		Term1_MoveTo(c2,r2);		Term1_SendFloatNum(CT2_Centre);
		Term1_MoveTo(c3,r2);		Term1_SendFloatNum(CT_Current[1]);
		//CT3 Values
		Term1_MoveTo(c1,r3);		Term1_SendFloatNum(CT3_max);
		Term1_MoveTo(c11,r3);		Term1_SendFloatNum(CT3_min);
		Term1_MoveTo(c2,r3);		Term1_SendFloatNum(CT3_Centre);
		Term1_MoveTo(c3,r3);		Term1_SendFloatNum(CT_Current[2]);
		//CT4 Values
		Term1_MoveTo(c1,r4);		Term1_SendFloatNum(CT4_max);
		Term1_MoveTo(c11,r4);		Term1_SendFloatNum(CT4_min);
		Term1_MoveTo(c2,r4);		Term1_SendFloatNum(CT4_Centre);
		Term1_MoveTo(c3,r4);		Term1_SendFloatNum(CT_Current[3]);
	}
/////////////////////////////////////// Main User Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

	/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

///////////////////////////////////// One Shot Operations \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

	//Disable the timers so they do not interrupt startup operations
	TI1_Disable();
	TI2_Disable();

	// Draws the terminal window
	drawGUI();     //uses functions
	Update_Putty_CT_Values(); // set initial values

	// Calibrate the ADC
	ADC_Calibrate(TRUE);

	// Turn off all bits
	CT1_BIT_PutVal(0); 	//ct1
	CT2_BIT_PutVal(0);	//ct2
	CT3_BIT_PutVal(0);	//ct3
	CT4_BIT_PutVal(0);	//ct4
	SLP_PutVal(0);		//zigbee
	MUXM_PutVal(0);		//mux to switch opamps

	// Start the Main Timer
	TI1_Enable(); //creates the first interrupt at 10 seconds

////////////////////////////////////// Primary FOR Loop \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/

	for (;;) { //for-loop-1

	// Check for the timer 1 interrupt and nothing has been typed
	if((timer1_interrupted == 0 )&& (hold == 0));{
		TI1_EnableEvent();//enables events from time 1 to count 5 second intervals
		  __asm("wfi"); //// wfi = "wait for interrupt" instruction puts the CPU in a low power state
	}


	// Check for timer1 interrupt and nothing has been typed into putty
	if  ((timer1_interrupted == 1) && (hold == 0)) {//if-main
		// This code creates a loop to count 250 samples of the sinewave
				TI2_Enable();//enable timer2
				 while(sample_index < number_samples){//collect the data samples at 1msec intervals
							 if(timer2_interrupted == 1 ){// activates on the interrput
								 MUXM_PutVal(1);//turn on mux in sampling loop
								 ADC_Measure(TRUE);//gets a new measurement
								 // Each channel is activated depending on which needs to be measured
											if ((ch1_measure) && (disable1 == 0)) {
												CT1_BIT_PutVal(1);//turn on ct op-amp for sampling
												ADC_GetChanValue16(3, &ADC_measure);
											CT1_Raw[sample_index] = ADC_measure; // pin 1
											} else if ((ch2_measure) && (disable2 == 0)) {
												CT2_BIT_PutVal(1);//turn on ct op-amp for sampling
												ADC_GetChanValue16(2, &ADC_measure);
											CT2_Raw[sample_index] = ADC_measure; // pin 2
											} else if ((ch3_measure) && (disable3 == 0)) {
												CT3_BIT_PutVal(1);//turn on ct op-amp for sampling
												ADC_GetChanValue16(0, &ADC_measure);
											CT3_Raw[sample_index] = ADC_measure; // pin 3
											} else if ((ch4_measure) && (disable4 == 0)) {
												CT4_BIT_PutVal(1);//turn on ct op-amp for sampling
												ADC_GetChanValue16(1, &ADC_measure);
											CT4_Raw[sample_index] = ADC_measure; // pin 4
											}//if2
								sample_index++;// increment the sample index until all samples have been taken
							 	 }//if1
					 timer2_interrupted = 0;//sets the interrupt flag back to zero
					//__asm("wfi");// wfi = "wait for interrupt" instruction puts the CPU in a low power state
							 }//while loop
			TI2_Disable();//stop timer2 from interrupting (turn off)
			sample_index = 0;//set the sample index back to zero to count for the next channel measurements

		// Out of data sampling loop, turn off and power to each opamp
			CT1_BIT_PutVal(0);
			CT2_BIT_PutVal(0);
			CT3_BIT_PutVal(0);
			CT4_BIT_PutVal(0);

		// 	Switch statement toggles through each CT to turn on and measure, disable and send data to the screen
			switch (CT_Counter) {
			case 1:
				//Disables the CT if not needed
				if (disable1 == 0){
					CT_Current[0] = RMS_calculator(&CT1_Raw);//calculates the RMS
					CT1_Centre = centre;
					CT1_min = min;
					CT1_max = max;
					// initiate a reenable counter
					reable_counter1 = 0;
								}
					// Test to see if the channel is measuring or can be shut down
							if ((CT_RMS < min_val) && (reable_counter1 == 0)) {
								disable1 = 1;
								reable_counter1 = 1;
								Term1_MoveTo(c4,r1);Term1_SendStr("OFF     ");
							} else if (reable_counter1 == drop_out) {
								disable1 = 0;
							} else if (gain_adjust == 1){
								Term1_MoveTo(c4,r1);Term1_SendStr("CLIPPED");//for when the gain is clipping
							} else {
								reable_counter1++;
								Term1_MoveTo(c4,r1);Term1_SendStr("ON     ");
							}
					//increment the counted to switch to next channel and turn on next channel
					CT_Counter++;
					ch1_measure = 0;
					ch2_measure = 1;
					ch3_measure = 0;
					ch4_measure = 0;

				break;
			case 2:
				//Disables the CT if not needed
				if (disable2 == 0){
					CT_Current[1] = RMS_calculator(&CT2_Raw);//calculates the RMS
					CT2_Centre = centre;
					CT2_min = min;
					CT2_max = max;
					// initiate a reenable counter
					reable_counter2 = 0;
								}
					// Test to see if the channel is measuring or can be shut down
								if ((CT_RMS < min_val) && (reable_counter2 == 0)) {
									disable2 = 1;
									reable_counter2 = 1;
									Term1_MoveTo(c4,r2);Term1_SendStr("OFF   ");
								} else if (reable_counter2 == drop_out) {
									disable2 = 0;
								} else if (gain_adjust == 1){
									Term1_MoveTo(c4,r2);Term1_SendStr("CLIPPED");//for when the gain is clipping
								} else {
								reable_counter2++;
								Term1_MoveTo(c4,r2);Term1_SendStr("ON     ");

								}
					//increment the counted to switch to next channel and turn on next channel
					CT_Counter++;
					ch1_measure = 0;
					ch2_measure = 0;
					ch3_measure = 1;
					ch4_measure = 0;

				break;
			case 3:
				//Disables the CT if not needed
					if (disable3 == 0){
						CT_Current[2] = RMS_calculator(&CT3_Raw);//calculates the RMS
						CT3_Centre = centre;
						CT3_min = min;
						CT3_max = max;
					// initiate a reenable counter
					reable_counter3 = 0;
							}
					// Test to see if the channel is measuring or can be shut down
							if ((CT_RMS < min_val) && (reable_counter3 == 0)) {
								disable3 = 1;
								reable_counter3 = 1;
								Term1_MoveTo(c4,r3);Term1_SendStr("OFF   ");
							} else if (reable_counter3 == drop_out) {
								disable3 = 0;
							} else if (gain_adjust == 1){
								Term1_MoveTo(c4,r3);Term1_SendStr("CLIPPED");//for when the gain is clipping
							} else {
							reable_counter3++;
								Term1_MoveTo(c4,r3);Term1_SendStr("ON     ");
							}
					//increment the counted to switch to next channel and turn on next channel
					CT_Counter++;
					ch1_measure = 0;
					ch2_measure = 0;
					ch3_measure = 0;
					ch4_measure = 1;

				break;
			case 4:
			//Disables the CT if not needed
			if (disable4 == 0){
				CT_Current[3] = RMS_calculator(&CT4_Raw);//calculates the RMS
				CT4_Centre = centre;
				CT4_min = min;
				CT4_max = max;
				// initiate a reenable counter
				reable_counter4 = 0;
				}
				// Test to see if the channel is measuring or can be shut down
							if ((CT_RMS < min_val) && (reable_counter4 == 0)) {
								disable4 = 1;
								reable_counter4 = 1;
								Term1_MoveTo(c4,r4);Term1_SendStr("OFF   ");
							} else if (reable_counter4 == drop_out) {
								disable4 = 0;
							} else if (gain_adjust == 1){
								Term1_MoveTo(c4,r4);Term1_SendStr("CLIPPED");//for when the gain is clipping
							} else {
							reable_counter4++;
							Term1_MoveTo(c4,r4);Term1_SendStr("ON     ");
							}
				//increment the counted to switch to next channel and turn on next channel
				CT_Counter = 1;
				ch1_measure = 1;
				ch2_measure = 0;
				ch3_measure = 0;
				ch4_measure = 0;


				// Send Over the Serial
				CT_Current[2] = 6.666; //set to value for testing
				CT_Current[3] = 0;//set to value for testing
								Term1_MoveTo(c1, 22);Term1_SendStr("CT_CURRENT DATA SENT  ");
								Term1_MoveTo(c1, 24);Term1_SendStr("CT_CURRENT_1         ");Term1_SendFloatNum(CT_Current[0]);
								Term1_MoveTo(c1, 25);Term1_SendStr("CT_CURRENT_2         ");Term1_SendFloatNum(CT_Current[1]);
								Term1_MoveTo(c1, 26);Term1_SendStr("CT_CURRENT_3         ");Term1_SendFloatNum(CT_Current[2]);
								Term1_MoveTo(c1, 27);Term1_SendStr("CT_CURRENT_4         ");Term1_SendFloatNum(CT_Current[3]);

									static char message [100];
									snprintf(message, 100, "%f,%f,%f,%f\n",CT_Current [0],CT_Current [1],CT_Current [2],CT_Current [3]);
									 int message_size = strlen(message);

						 // Construct the Xbee API frame packet
									  byte packet [128];
									  packet[0] = 0x7E; // Start delimiter
									  packet[1] = 0x00; packet[2] = (byte)(message_size + 14); // Frame length
									  packet[3] = 0x10; // Frame type: Transmit request
									  packet[4] = 0x01; // Frame ID
									  packet[5] = 0x00; packet[6] = 0x00;
									  packet[7] = 0x00; packet[8] = 0x00;
									  packet[9] = 0x00; packet[10] = 0x00;
									  packet[11] = 0xFF; packet[12] = 0xFF; // 64 bit destination address: Broadcast
									  packet[13] = 0xFF; packet[14] = 0xFE; // 16 bit destination address: Broadcast
									  packet[15] = 0x00; // Broadcast radius
									  packet[16] = 0x00; // Options: None

									  // Place message into frame packet
									  for (int i = 0; i < message_size; i++) {
										  packet[17 + i] = (byte)message[i];
									  }

									  // Xbee API checksum calculation
									  uint8 checksum = 0xFF;
									  for (int i = 3; i < 17 + message_size; i++) {
										  checksum -= (uint8)packet[i];
									  }
									  packet[17 + message_size] = checksum;


									  // Transmit one byte at a time
									  for(int i = 0; i < sizeof(packet); i++) {
										  while(AS1_SendChar((byte)packet[i]) != ERR_OK) {}
									  }

									  // Update values on GUI
									  				Update_Putty_CT_Values();


				//Reset flags, turn off and save power
				timer1_interrupted = 0;
				MUXM_PutVal(0); 	//turn off mux
				SLP_PutVal(0);//turn of zigbee



				break;
			}









	//If command is reciered it sends it to the functions above to check what to do
					if ((command_recieved == 1) & (command_sent == 0)) {//Recieve Flag
					CommandReact(); //function uses logic above
					command_sent = 0;
					command_recieved = 0;
				}//if-flag


	}// if-main
	}//end-for-loop-1
 /////////////////////////////////// End Main User Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/



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

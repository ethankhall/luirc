#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <syslog.h>

/*	This is the part of the code that lets you define what keys are what
 *	if you don't do this properly then nothing will work right in the code.
 *	We are going to assume that you are using an Insignia remote (what my
 *	tv is) and these are the basic buttons.. The acronyms are self explanitory
 *	so if you have any problems then you can always redefine them.  The if/else
 *	block later will tell you how they are used.
 */
#define UPARROW "61A042BD"
#define DOWNARROW "61A0C23D"
#define LEFTARROW "61A06897"
#define RIGHTARROW "61A0A857"
#define ENTER "61A018E7"
#define EXIT "61A0D827"
#define CHUP "61A050AF"
#define CHDOWN "61A0D02F"

/*
 *	This section will hold global variables that will be used in the program.
 *	These will be things like debug enables and adding different modes.
 */

int debugEnable = 0;
char device[30]; //The device that we will be using for the program.

/*
 *	END OF THE GLOBAL VARIABLES
 */


int main(int argc, char* argv[]){
	int i = 0;
	FILE * filetoopen;	//File pointer to the serial (usb serial?) device
	char output[9];	//the output from the serial connection
	char * temp; //TODO: Delete this line

	//Command line pass in options
	for(i = 1; i < argc; i++){
		//
		//	BEGIN OF DEBUG ENABLE SECTION
		//
		
		//This will allow a user to spit out debug info and there will be a mode
		//that will spit out what the micro is sending and it won't processes
		//any of it.  That will be the first step twards a learning mode.

		if(!strcmp(argv[i], "--debug")){
			debugEnable = 1;
		}
		
		//
		//	END OF DEBUG ENABLE SECTION
		//
	}

	//
	//	THIS SECTION OF THE CLI PASS IN'S HAVE DEPENDENCIES ON THE PREVIOUS 
	//	ONES, ORDER MAY MATTER
	//

	for(i = 1; i < argc; i++){

		//
		// BEGIN OF DEVICE SELECT SECTION
		//

		//Check for the '-d' option

		//From strcmp definition:
		//	A zero (return) value indicates that both strings are equal.
		if(!strcmp(argv[i], "-p")){
			
			//Manually incroment 'i' because the format that the program uses from
			//the command is like "program_name -d /dev/ttyUSB0"
			//
			//This will also break the program if they did not enter the correct
			//number of arguments into the program. Well atleast if they did not 
			//include the device name in the execution.

			if(i < argc){ 
				i++;
			} else { 
				printf("You did not enter the device name. Please ");
				printf("include this after '-d'.  Read the manual for help\n");
				break;
			}

			//Makes sure that they did not forget to input the device name
			if(argv[i][0] == '-'){
				printf("Im sorry but you did not enter a valid device name.  ");
				printf("This should be in the format of '/dev/foo'.\n");
			} else {
				//I am assuming this value works, the while loop will check to to help
				//protect against seg faulting.

				strncpy(device, argv[i], strlen(argv[i]));
				device[strlen(argv[i])] = '\0'; //Terminate the string.
				if(debugEnable) { printf("strlen: %d\n", strlen(argv[i])); } //<-- Debug
				if(debugEnable) { printf("device name is: %s\n", device); } //<--- Debug
			}
		} else {
			//Sets the default to '/dev/ttyUSB0' since that is what it will be when
			//I use it.. I can change this easily
			strcpy(device, "/dev/ttyUSB0");
		}
	}
		//
		//	END OF DEVICE SELECT SECTION
		//
	
	if(debugEnable) { printf("device name is: %s\n", device); } //<--- Debug
	filetoopen = fopen(device, "r");

	//This next statement is not needed by the program but it is nice to have
	//so that the user is able to see what they are connecting to, so they will
	//know if they have any problems.
	
	if(debugEnable) printf("The device I am trying to connect to is: %s\n", device);	

	//Start up syslog data
	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("luirc", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_INFO, "<luirc started with process number %d", getuid() );
	closelog ();
	
	//This loop will take in 9 chars. I need this so that I can eat the 
	//newline ('\n') from the micro that is output. I have the micro outputing 
	//the new line so that it is eaiser to read. The micro will always (unless
	//something major happens) will output a 32bit hex value. Then it will send 
	//out a newline so there is a total of 9 chars.
	
	if(!filetoopen){
		printf("I'm sorry but I could not connect to your serial device...\n");
		printf("Program exiting with nothing left to do...\n");
		printf("Goodbye... :-)\n");
		syslog (LOG_NOTICE, "Sorry I could not open the device, exiting..");
		return -1;
	}
	
	while (filetoopen){
			for(i = 0; i < 9; i++){
			output[i] = getc(filetoopen);
			//printf("%c", output[i]);  //<-- Debuging statement!
		}

		//Null terminate the string.. I will always get 8 chars and so i made the
		//string 9 so that I would be able to add a '\0' to the end of it so that
		//I can create a little give way if I ever need to do string munipulation.
		//
		//This will also eat the '\n' that is spit out from the output
	
		output[8] = '\0';
		if(strstr(output, UPARROW)){ 
			if(debugEnable) printf("Uparrow\n");
			system("echo \"UP\" > /dev/virtkey");
		} else if (strstr(output, DOWNARROW)){
			if(debugEnable) printf("Down\n");
			system("echo \"DOWN\" > /dev/virtkey");
		} else if (strstr(output, LEFTARROW)){
			if(debugEnable) printf("Left\n");
			system("echo \"LEFT\" > /dev/virtkey");
		} else if (strstr(output, RIGHTARROW )){
			if(debugEnable) printf("Right\n");
			system("echo \"RIGHT\" > /dev/virtkey");
		} else if (strstr(output, ENTER)){
			if(debugEnable) printf("Enter\n");
			system("echo \"ENTER\" > /dev/virtkey");
		} else if (strstr(output, EXIT)){
			if(debugEnable) printf("Exit\n");
			system("echo \"ESC\" > /dev/virtkey");
		} else if (strstr(output, CHUP)){
			if(debugEnable) printf("CH UP\n");
			system("echo \"PAGEUP\" > /dev/virtkey");
		} else if (strstr(output, CHDOWN)) {
			if(debugEnable) printf("CH DOWN\n");
			system("echo \"PAGEDOWN\" > /dev/virtkey");
		} else {
			if(degudEnable) syslog (LOG_NOTICE, "Got an invalid key %s", output);
		}
	}
	return 0;
}

//End of Program


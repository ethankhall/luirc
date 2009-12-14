#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <termios.h>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>  /* _exit, fork */
#include <errno.h>   /* errno */

/*	This is the pcaart of the code that lets you define what keys are what
 *	if you don't do this properly then nothing will work right in the code.
 *	We are going to assume that you are using an Insignia remote (what my
 *	tv is) and these are the basic buttons.. The acronyms are self explanitory
 *	so if you have any problems then you can always redefine them.  The if/else
 *	block later will tell you how they are used.
 */

#define VERSION_NUMBER "0.1"
#define UPARROW "61A042BD"
#define DOWNARROW "61A0C23D"
#define LEFTARROW "61A06897"
#define RIGHTARROW "61A0A857"
#define ENTER "61A018E7"
#define EXIT "61A0D827"
#define CHUP "61A050AF"
#define CHDOWN "61A0D02F"

void cleanup(int signal);

/*
 *	This section will hold global variables that will be used in the program.
 *	These will be things like debug enables and adding different modes.
 */

static int verbose_flag;
static int deamon_flag = 1;
char device[30] = "/dev/ttyUSB0"; //The device that we will be using for the program.
FILE * filetoopen;	//File pointer to the serial (usb serial?) device
FILE * deviceDriver;
/*
 *	END OF THE GLOBAL VARIABLES
 */

//Handel when the program has to exit....
void  cleanup(int signal){
	if (verbose_flag) printf("Exiting program...\nGoodbye!\n");
	printf("\n");
	syslog (LOG_NOTICE, "luirc deamon exiting nicely...");
	closelog ();
	if (filetoopen)
	{
		fclose(filetoopen);
	}
	if ( deviceDriver ){
		fclose(deviceDriver);
	}
	return;
}



int main(int argc, char* argv[]){
	int i = 0;
	char output[9];	//the output from the serial connection
	int command;
	int option_index;
	
	//
	//	Signal Stuff (closing files on program close)
	//
	
	signal(SIGINT,cleanup); 
	signal(SIGTERM,cleanup); 
	
	static struct option long_options[] =
	{
		/* These options set a flag. */
		{"verbose", no_argument,       &verbose_flag, 1},
		{"debug", 	no_argument, 	&verbose_flag, 1},
		{"d",  			no_argument,  &verbose_flag, 1},
		{"port",  	required_argument, 0, 'a'},
		{"p",		  	required_argument, 0, 'a'},
		{"no-deamon", no_argument, &deamon_flag, 0},
		{"version", no_argument, 0, 'b'},
		{"v", 			no_argument, 0, 'b'},
		{"h", 			no_argument, 0, 'h'},
		{"help", 			no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	while (1){
	
		option_index = 0;
		
		command = getopt_long (argc, argv, "ab",
										long_options, &option_index);

		if (command == -1)
			break;
			
		switch(command){
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'a':
				strncpy(device, argv[i], strlen(argv[i]));
				device[strlen(argv[i])] = '\0'; //Terminate the string.
				if(verbose_flag) { printf("strlen: %d\n", strlen(argv[i])); } //<-- Debug
				if(verbose_flag) { printf("device name is: %s\n", device); } //<--- Debug
				strncpy(device, optarg, strlen(optarg));
				break;
			case 'b':
				printf("This software is made by Ethan Hall.\n\n");
				printf("Version number: %s\n", VERSION_NUMBER);
				return 1;
				break;
			default:
			case 'h':
				printf("Usage: (--v|--version) --verbose  --(d|debug) --(p|port) <device> --(h|help) --no-deamon\n");
				printf("Exiting...\n");
				return -1;
				break;
		}
	}
	
	
	
	
	
	if(verbose_flag) { printf("device name is: %s\n", device); } //<--- Debug
	filetoopen = fopen(device, "r");

	//This next statement is not needed by the program but it is nice to have
	//so that the user is able to see what they are connecting to, so they will
	//know if they have any problems.
	
	if(verbose_flag) printf("The device I am trying to connect to is: %s\n", device);	

	//Start up syslog data
	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("luirc", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_INFO, "< luirc started with process number %d", getuid() );
	//closelog ();
	
	//This loop will take in 9 chars. I need this so that I can eat the 
	//newline ('\n') from the micro that is output. I have the micro outputing 
	//the new line so that it is eaiser to read. The micro will always (unless
	//something major happens) will output a 32bit hex value. Then it will send 
	//out a newline so there is a total of 9 chars.
	
	if(!filetoopen){
		if(verbose_flag){
			printf("I'm sorry but I could not connect to your serial device...\n");
			printf("Program exiting with nothing left to do...\n");
			printf("Goodbye... :-)\n");
		}
		syslog (LOG_NOTICE, "Sorry I could not open the device, exiting..");
		raise(SIGTERM);
	}
	
	if(deamon_flag){
		if(fork() > 0){
			syslog (LOG_NOTICE, "< deamonizing luirc >");
			exit(0);
			//Parent Exits
		}
	}
	
	setsid();
	chdir("/tmp");
	
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
		deviceDriver = fopen("/dev/luirc", "w");
		if(deviceDriver == 0){
			syslog (LOG_NOTICE, "Input device not accessable, exiting..");
			raise(SIGTERM);
		}
		output[8] = '\0';
		if(strstr(output, UPARROW)){ 
			if(verbose_flag) printf("Uparrow\n");
			//Send the up arrow key
			fprintf(deviceDriver, "%c", (char)103);
			
		} else if (strstr(output, DOWNARROW)){
			if(verbose_flag) printf("Down\n");
			//Send the down arrow key
			fprintf(deviceDriver, "%c", (char)108);
			
		} else if (strstr(output, LEFTARROW)){
			if(verbose_flag) printf("Left\n");
			//Send the left arrow key
			fprintf(deviceDriver, "%c", (char)105);
			
		} else if (strstr(output, RIGHTARROW )){
			if(verbose_flag) printf("Right\n");
			//Send the right arrow key
			fprintf(deviceDriver, "%c", (char)106);
			
		} else if (strstr(output, ENTER)){
			if(verbose_flag) printf("Enter\n");
			//Send the enter key
			fprintf(deviceDriver, "%c", (char)28);
			
		} else if (strstr(output, EXIT)){
			if(verbose_flag) printf("Exit\n");
			//Send the escape key
			fprintf(deviceDriver, "%c", (char)1);
			
		} else if (strstr(output, CHUP)){
			if(verbose_flag) printf("CH UP\n");
			//Send the pageup key
			fprintf(deviceDriver, "%c", (char)104);
			
		} else if (strstr(output, CHDOWN)) {
			if(verbose_flag) printf("CH DOWN\n");
			fprintf(deviceDriver, "%c", (char)109);
		} else {
			if(verbose_flag) syslog (LOG_NOTICE, "Got an invalid key %s", output);
		}
		fclose(deviceDriver);
	}
	return 0;
}


//End of Program


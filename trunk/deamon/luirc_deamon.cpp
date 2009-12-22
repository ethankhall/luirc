#include "luirc_deamon.h"

static int verbose_flag;
static int deamon_flag = 1;

//Variables provided by preferences.h

 FILE * preferences;

using namespace boost;
namespace programOptions = boost::program_options;

using namespace std;

/*
*	This section will hold global variables that will be used in the program.
*	These will be things like debug enables and adding different modes.
*/


FILE * filetoopen;	//File pointer to the serial (usb serial?) device
FILE * deviceDriver;
string device = "/dev/ttyUSB0"; //The device that we will be using for the program.

string prefArray[] = {	"61A042BD", "61A0C23D", "61A06897", "61A0A857", 
"61A018E7", "61A0D827", "61A050AF", "61A0D02F" };

string prefIndex[] = {"UPARROW", "DOWNARROW", "LEFTARROW", "RIGHTARROW",
"ENTER", "EXIT", "CHUP", "CHDOWN" };

int main(int argc, char *argv[]){
	
	int i = 0;
	string output;	//the output from the serial connection
	int command;
	int option_index;
	
	preferences = fopen("luirc.conf", "r");
	
	signal(SIGINT,cleanup); 
	signal(SIGTERM,cleanup); 
	
	// Declare the supported options.
	programOptions::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Print this message")
		("verbose", "Be verbos")
		("debug,d", 	"Print Debug Messages")
		("port,p", programOptions::value<string>(), "Port that is going to be used (ie. /dev/ttyFoo)")
		("no-deamon", "Des not deamonize the process (Great for debugging)")
		("version,v", "Print out the version")
;

	programOptions::variables_map vm;
	programOptions::store(programOptions::parse_command_line(argc, argv, desc), vm);
	programOptions::notify(vm);    

	if (vm.count("help")) {
		//Print out description
		cout << desc << "\n";
		return 1;
	}

	if (vm.count("verbose")) {
		verbose_flag = 1;
	}
	
	if(vm.count("no-deamon")){
		deamon_flag = 0;
	}
	
	if(vm.count("port")){
		device = vm["port"].as<string>();
	}
	
	if(vm.count("version")){
		printf("Version: %s\n", VERSION_NUMBER);
		return -1;
	}
	
	
	
	//Begin functionality
	if(verbose_flag){
		printf("Here are the preferences that you have loaded:\n");
		for(i = 0; i < 8; i++){
			printf("prefArray[%s] = \"%s\"\n", prefIndex[i].c_str(), prefArray[i].c_str());
		}
	}
	
	
	
	if(verbose_flag) { printf("device name is: %s\n", device.c_str()); } //<--- Debug
		filetoopen = fopen(device.c_str(), "r");
		
		//This next statement is not needed by the program but it is nice to have
		//so that the user is able to see what they are connecting to, so they will
		//know if they have any problems.
		
		if(verbose_flag) printf("The device I am trying to connect to is: %s\n", device.c_str());	
		
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
			output.clear();
			for(i = 0; i < 9; i++){
				output += getc(filetoopen);
				//printf("%c", output[i]);  //<-- Debuging statement!
			}
			//if(verbose_flag) printf("input: %s\n", output.c_str());
			
			//Null terminate the string.. I will always get 8 chars and so i made the
			//string 9 so that I would be able to add a '\0' to the end of it so that
			//I can create a little give way if I ever need to do string munipulation.
			//
			//This will also eat the '\n' that is spit out from the output
			deviceDriver = fopen("/dev/luirc", "w");
			if(deviceDriver == 0){
				syslog (LOG_NOTICE, "Input device not accessable, exiting..");
				printf("Sorry can't open the device driver, exiting...\n");
				raise(SIGTERM);
			}
			if(output.find(prefArray[UPARROW]) == 0){ 
				if(verbose_flag) printf("Uparrow\n");
				//Send the up arrow key
				fprintf(deviceDriver, "%c", (char)103);
				
			} else if(output.find(prefArray[DOWNARROW]) == 0){
				if(verbose_flag) printf("Down\n");
				//Send the down arrow key
				fprintf(deviceDriver, "%c", (char)108);
				
			} else if(output.find(prefArray[LEFTARROW]) == 0){
				if(verbose_flag) printf("Left\n");
				//Send the left arrow key
				fprintf(deviceDriver, "%c", (char)105);
				
			} else if(output.find(prefArray[RIGHTARROW]) == 0){
				if(verbose_flag) printf("Right\n");
				//Send the right arrow key
				fprintf(deviceDriver, "%c", (char)106);
				
			} else if(output.find(prefArray[ENTER]) == 0){
				if(verbose_flag) printf("Enter\n");
				//Send the enter key
				fprintf(deviceDriver, "%c", (char)28);
				
			} else if(output.find(prefArray[EXIT]) == 0){
				if(verbose_flag) printf("Exit\n");
				//Send the escape key
				fprintf(deviceDriver, "%c", (char)1);
				
			} else if(output.find(prefArray[CHUP]) == 0){
				if(verbose_flag) printf("CH UP\n");
				//Send the pageup key
				fprintf(deviceDriver, "%c", (char)104);
				
			} else if(output.find(prefArray[CHDOWN]) == 0) {
				if(verbose_flag) printf("CH DOWN\n");
				fprintf(deviceDriver, "%c", (char)109);
			} 
			fclose(deviceDriver);
		}
		return 0;
}

//Handel when the program has to exit....
void  cleanup(int signal){
	if (verbose_flag) printf("Exiting program...\nGoodbye!\n");
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


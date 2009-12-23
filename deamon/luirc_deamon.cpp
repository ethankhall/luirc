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
	
	map<string,string> preferences;
	map<string,string>::iterator itter;
	map<string,char>::iterator itter_info;
	map<string,char> preferences_info;
	
	preferences.insert ( pair<string,string>("61A042BD","UPARROW") );
	preferences.insert ( pair<string,string>("61A0C23D","DOWNARROW") );
	preferences.insert ( pair<string,string>("61A06897","LEFTARROW") );
	preferences.insert ( pair<string,string>("61A0A857","RIGHTARROW") );
	preferences.insert ( pair<string,string>("61A018E7","ENTER") );
	preferences.insert ( pair<string,string>("61A0D827","EXIT") );
	preferences.insert ( pair<string,string>("61A050AF","CHUP") );
	preferences.insert ( pair<string,string>("61A0D02F","CHDOWN") );
	
	preferences_info.insert ( pair<string,char>("UPARROW",(char)103) );
	preferences_info.insert ( pair<string,char>("DOWNARROW",(char)108) );
	preferences_info.insert ( pair<string,char>("LEFTARROW",(char)105) );
	preferences_info.insert ( pair<string,char>("RIGHTARROW",(char)106) );
	preferences_info.insert ( pair<string,char>("ENTER",(char)28) );
	preferences_info.insert ( pair<string,char>("EXIT",(char)1) );
	preferences_info.insert ( pair<string,char>("CHUP",(char)104) );
	preferences_info.insert ( pair<string,char>("CHDOWN",(char)109) );
	
	//preferences = fopen("luirc.conf", "r");
	
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
	programOptions::store(programOptions::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
	programOptions::notify(vm);  
	
	
	if (vm.count("help")) {
		//Print out description
		cout << desc << "\n";
		
		printf("Note: Any unknown options will be ignored\n");
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
		return 1;
	}
	
	//Begin functionality
	if(verbose_flag){
		printf("Here are the preferences that you have loaded:\n");
		printf("{input string} \t= {key press}\n");
		for(itter=preferences.begin(); itter != preferences.end(); ++itter){
			cout << "[" << itter->first << "] \t= " << itter->second << endl;
		}
		
		//Old version: TODO Remove below
		//for(i = 0; i < 8; i++){
		//	printf("prefArray[%s] = \"%s\"\n", prefIndex[i].c_str(), prefArray[i].c_str());
		//}
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
			char inputChar;
			inputChar = getc(filetoopen);
			
			do{
				output += inputChar;
				inputChar = getc(filetoopen);
				//printf("%c", output[i]);  //<-- Debuging statement!
			}while(inputChar != '\n');
			//if(verbose_flag) printf("input: %s\n", output.c_str());
			//This will also eat the '\n' that is spit out from the output
			
			
			deviceDriver = fopen("/dev/luirc", "w");
			if(deviceDriver == 0){
				syslog (LOG_NOTICE, "Input device not accessable, exiting..");
				printf("Sorry can't open the device driver, exiting...\n");
				raise(SIGTERM);
			}
			
			//These section now use's a map to make things easier to do configurations.
			if(verbose_flag) printf("searching for %s\n", output.c_str());
			itter = preferences.find(output.c_str());
			if(itter != preferences.end()){
				
				if(verbose_flag) { cout << "Command is: " << itter->second << endl; }
				
				//get the char to send
				itter_info = preferences_info.find(itter->second);
				if(itter_info != preferences_info.end()){
					
					inputChar = itter_info->second;
					fprintf(deviceDriver, "%c", inputChar);
				} else {
					printf("Sorry I don't know that command\n");
					syslog (LOG_NOTICE, "Invalid command, please check your config");
				}
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
	exit(1);
}


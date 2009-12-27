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
	std::string configFilename;
	
	RemotePreferences prefs;
	ButtonPreferences pref_settings;
	
	map<string,char> preferences_info;
	map<string,char>::iterator itter_info;
	
	/*
	prefs.insertToHash("61A042BD","UPARROW");
	prefs.insertToHash("61A0C23D","DOWNARROW");
	prefs.insertToHash("61A06897","LEFTARROW");
	prefs.insertToHash("61A0A857","RIGHTARROW");
	prefs.insertToHash("61A018E7","ENTER");
	prefs.insertToHash("61A0D827","EXIT");
	prefs.insertToHash("61A050AF","CHUP");
	prefs.insertToHash("61A0D02F","CHDOWN");
	*/
	
	pref_settings.insertToHash("UPARROW",(char)103);
	pref_settings.insertToHash("DOWNARROW",(char)108);
	pref_settings.insertToHash("LEFTARROW",(char)105);
	pref_settings.insertToHash("RIGHTARROW",(char)106);
	pref_settings.insertToHash("ENTER",(char)28);
	pref_settings.insertToHash("EXIT",(char)1);
	pref_settings.insertToHash("CHUP",(char)104);
	pref_settings.insertToHash("CHDOWN",(char)109);
	
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
		("config,c", programOptions::value<string>()->default_value(DEFAULT_OPTION_FILE), "Config file")
;
	programOptions::positional_options_description p;
	p.add("programOptions", -1);

	programOptions::variables_map vm;
	programOptions::store(programOptions::command_line_parser(argc, argv).options(desc).positional(p).allow_unregistered().run(), vm);
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
	
	if (vm.count("config")){
		configFilename = vm["config"].as<string>();
	}
	
	getConfigOptions(prefs, configFilename);
	
	//Begin functionality
	if(verbose_flag){
		printf("Here are the preferences that you have loaded:\n");
		printf("{input string} \t= {key press}\n");
		prefs.displayMap();
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
		
		//allow the program to fork off and create a deamon
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
			
			//Open the device driver, and error out if the driver doesn't exist.
			//It will put an error message out onto the screen and it will put it in the
			// error log
			deviceDriver = fopen("/dev/luirc", "w");
			if(deviceDriver == 0){
				syslog (LOG_NOTICE, "Input device not accessable, exiting..");
				printf("Sorry can't open the device driver, exiting...\n");
				raise(SIGTERM);
			}
			
			//These section now use's a map to make things easier to do configurations.
			if(verbose_flag) printf("searching for %s\n", output.c_str());
			
			output = prefs.searchHash(output);
			
			if(!output.find("")){
				
				//Command executed if 'verbos'
				if(verbose_flag) { cout << "Command is: " << output << endl; }
				
				//get the char to send
				inputChar = pref_settings.searchHash(output);
				if(inputChar != 0){
					fprintf(deviceDriver, "%c", inputChar);
				} else {
					if(verbose_flag){
						printf("Sorry I don't know that command\n");
						syslog (LOG_NOTICE, "Invalid command, please check your config");
					}
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


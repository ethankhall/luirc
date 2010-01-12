#ifndef _preferences_hpp_
#define _preferences_hpp_

#include <string>
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>
//#include "key_defines.hpp"

#define VERSION_NUMBER "0.2"

class RemotePreferences {
	std::map<std::string,std::string> mapData;
	std::map<std::string,std::string>::iterator mapData_itter;
	public:
		void insertToHash(std::string key, std::string value);
		void displayMap(void);
		std::string searchHash(std::string key);
};

class ButtonPreferences {
	std::map<std::string,char> mapData;
	std::map<std::string,char>::iterator mapData_itter;
	public:
		void insertToHash(std::string key, char value);
		void displayMap(void);
		char searchHash(std::string key);
		std::string getKeys(void);
};

void getConfigOptions(RemotePreferences & prefs, std::string fileName);
void setNewConfig(RemotePreferences & prefs, ButtonPreferences keys, std::string fileName);

#endif
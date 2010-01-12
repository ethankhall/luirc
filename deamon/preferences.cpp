#include "preferences.hpp"

using namespace std;
void setNewConfig(RemotePreferences & prefs, ButtonPreferences keys, std::string fileName){
	int colCount = 1;
	int colCountSave;
	char temp[2];
	string inputString;
  string keyButtons = keys.getKeys();
	
	keyButtons[keyButtons.length() - 1] = '\0';	//Terminate String, removing the trailing ':'
	for(int i = 0; i < keyButtons.length(); i++){
		if(keyButtons[i] == ':'){
			colCount++;
		}
	}
	colCountSave = colCount;
	char * strings[colCountSave];
	for ( int i = 0; i < colCountSave; i++){
		strings[i] = (char *) calloc(50, sizeof(char));
		strcpy(strings[i], "");
	}
	colCount = 0;
	for(int i = 0; i < keyButtons.length(); i++){
		if ( keyButtons[i] != ':' ){
			keyButtons.copy(temp,1,i);
			temp[1] = '\0';
			strcat(strings[colCount], temp);
		} else {
			colCount++;
		}
	}
	
	//Got an array of all the possible buttons
	while(1){
		printf("Please enter the number that matches they key you want to use\n");
		for(int i = 0; i < colCountSave; i++){
			printf("%d - %s\n", i, strings[i]);
		}
		printf("-1 to exit\n");
		printf("Input: ");
		cin >> inputString;
		if(!inputString.find("-1")){
			//This will break out of the loop
			break;
		}
	}
  return;
}

void getConfigOptions(RemotePreferences & prefs, std::string fileName){
	ifstream fp;
	fp.open (fileName.c_str());
	
	std::string key, value, inputC;
	
	while(fp.good()){
		key.clear();
		value.clear();
		inputC.clear();
		do{
			key = key + inputC;
			inputC = fp.get();
		}while(inputC.find('='));
		inputC.clear();
		do{
			value = value + inputC;
			inputC = fp.get();
		}while(inputC.find("\n") && fp.good());
		prefs.insertToHash(key,value);
	}
	return;
}

/*
 *	ButtonPreferences class functions
 */

string ButtonPreferences::getKeys(void){
	string Keys;
	Keys.clear();
	for ( mapData_itter=mapData.begin() ; mapData_itter != mapData.end(); mapData_itter++ ){
		Keys.append((*mapData_itter).first);
		Keys.append(":");
	}
	return Keys;
}
void ButtonPreferences::insertToHash(std::string key, char value){
	mapData.insert ( pair<std::string,char>(key,value) );
	return;
}

void ButtonPreferences::displayMap(void){
	for ( mapData_itter=mapData.begin() ; mapData_itter != mapData.end(); mapData_itter++ ){
		cout << (*mapData_itter).first << " => " << (*mapData_itter).second << endl;
	}
	return;
}

char ButtonPreferences::searchHash(std::string key){
	mapData_itter = mapData.begin();
	mapData_itter = mapData.find(key);
	if (mapData_itter != mapData.end() )
	{
		return mapData_itter->second;
	} else {
		return 0;
	}
}


/*
 *	RemotePreferences class functions
 */

std::string RemotePreferences::searchHash(std::string key){
	mapData_itter = mapData.begin();
	mapData_itter = mapData.find(key);
	if (mapData_itter != mapData.end() )
	{
		return mapData_itter->second;
	} else {
		return "";
	}
}

void RemotePreferences::insertToHash(std::string key, std::string value){
	mapData.insert ( pair<std::string,std::string>(key,value) );
	return;
}

void RemotePreferences::displayMap(void){
	for ( mapData_itter=mapData.begin() ; mapData_itter != mapData.end(); mapData_itter++ ){
		cout << (*mapData_itter).first << " => " << (*mapData_itter).second << endl;
	}
	return;
}
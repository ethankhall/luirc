#include "preferences.hpp"

using namespace std;

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
		//cout << "Key = " << key << "\tValue = " << value << endl;
		prefs.insertToHash(key,value);
	}
	return;
}

/*
 *	ButtonPreferences class functions
 */
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
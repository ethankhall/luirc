#include "preferences.h"

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
* 			Globals
*/

char prefArray[][10] = {	"61A042BD", "61A0C23D", "61A06897", "61A0A857", 
"61A018E7", "61A0D827", "61A050AF", "61A0D02F" };

char prefIndex[][10] = {"UPARROW", "DOWNARROW", "LEFTARROW", "RIGHTARROW",
"ENTER", "EXIT", "CHUP", "CHDOWN" };

/*
*	This section will hold global variables that will be used in the program.
*	These will be things like debug enables and adding different modes.
*/

FILE * preferences;

void getPreferences(void){
	fscanf(preferences, "<UPARROW=%s>", prefArray[UPARROW]);
	fscanf(preferences, "<DOWNARROW=%s>", prefArray[DOWNARROW]);
	fscanf(preferences, "<LEFTARROW=%s>", prefArray[LEFTARROW]);
	fscanf(preferences, "<RIGHTARROW=%s>", prefArray[RIGHTARROW]);
	fscanf(preferences, "<ENTER=%s>", prefArray[ENTER]);
	fscanf(preferences, "<EXIT=%s>", prefArray[EXIT]);
	fscanf(preferences, "<CHUP=%s>", prefArray[CHUP]);
	fscanf(preferences, "<CHDOWN=%s>", prefArray[CHDOWN]);
	
	return;
}
/*
 * example.cpp for class DCF77Decoder
 * 
 * Copyright 2014 Jan "aso" Szenborn <aso.gdynia@gmail.com>
 * 
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, 
 * including commercial applications, and to alter it and redistribute it freely, 
 * subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software.
 *    If you use this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required. 
 * 
 * 2. Altered source versions must be plainly marked as such, 
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 */
#include <iostream>
#include <string>
#include "dcf77decoder.cpp"

using namespace std;

const string weekdays[7] = {
	"monday",
	"tuesday",
	"wednesday",
	"thursday",
	"friday",
	"saturday",
	"sunday"
};

int main() {
	/*
	 * Example test data:
	 * 00011110001100100010110001000010010010010000110000001010000
	 */
	
	// Load data from stdin
	string data;
	cout << "Data (59 bits): ";
	cin >> data;
	
	// Create DCF77Decoder object and load data
	DCF77Decoder decoder;
	bool ok = decoder.set(data);
	if (!ok) {
		// When set() returns false, it means that data is incorrect
		cout << "Incorrect data. Receive new frame and try again." << endl;
		return 1;
	}
	
	// Get result
	DCF77Result result = decoder.getResult();
	
	// Print
	cout << "== Received data ==" << endl
		 << "Date: " << result.time.day << "." << result.time.month << "." << result.time.year << endl
		 << "It's " << result.time.weekday << " day of week (" << weekdays[result.time.weekday-1] << ")" << endl
		 << "Hour: " << result.time.hour << ":" << result.time.minute << endl
		 << "Time: ";
	
	if (result.summerTime == 0)
		cout << "summer";
	else
		cout << "winter";
	
	if (result.timeChange != 0) 
		cout << " (next hour is time change)";

	cout << endl << "Antenna: " << result.antenna << " (";
	
	if (result.antenna == 0) cout << "normal"; else cout << "backup";
	cout << ")" << endl << "Weather information is crypted :(";
	
	return 0;
}

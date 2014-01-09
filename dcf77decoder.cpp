/*
 * dcf77decoder.cpp
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

struct DCF77Time {
	/*
	 * Struct to store DCF77 time
	 */
	unsigned short hour		= 0,
				   minute	= 0,
				   day		= 0,
				   weekday	= 0,
				   month	= 0,
				   year		= 0;
	   
	void clear() {
		/* Simple clear function */
		hour = 0; minute = 0;
		day = 0; weekday = 0;
		month = 0; year = 0;
	}
};


struct DCF77Result {
	/*
	 * Struct to store result from decoder
	 */
	DCF77Time time;					// Date and time
	unsigned short antenna = 0,		// 0 - primary, 1 - secondary (or service maintenance)
				   timeChange = 0,	// Next hour is the time change (CET/CEST)
				   summerTime = 0;	// 1 when CEST time
};


class DCF77Decoder {
	/*
	 * name: DCF77Decoder
	 * description: decoder for bits received from DCF77
	 * usage: create object, use set(param) function and getResult()
	 */
	private:
		bool data[59] = {false};		// Binary data
		DCF77Result result;	
		unsigned short bcd2dec(bool one, bool two, bool three, bool four);
		bool parityBit(bool *data, unsigned short startIndex, unsigned short endIndex);
	public:
		DCF77Decoder();
		
		// Overloaded functions for many data types
		bool set(bool data[59]);
		bool set(unsigned short data[59]);
		bool set(std::string data);
		
		DCF77Result getResult();
};


inline unsigned short DCF77Decoder::bcd2dec(bool one, bool two, bool three, bool four) {
	/*
	 * Converts 4-bits data in BCD format to decimal number (range 0-9)
	 * Order: little endian
	 */
	return (one*8 + two*4 + three*2 + four*1);
}


bool DCF77Decoder::parityBit(bool *data, unsigned short startIndex, unsigned short endIndex) {
	/*
	 * Returns value of calculated parity bit from range in data array
	 * true is 1, false is 0
	 */
	 
	// Just can't proceed when incorrect data
	if (startIndex > endIndex)
		return false;
		
	// Count of true bits (1)
	unsigned char count = 0;
	
	// Iterate over data for count true bits
	for (; startIndex <= endIndex; startIndex++)
		if (data[startIndex] == 1)
			count++;
	
	// When count is even, return 0, else return 1
	if (count%2 == 0)
		return false;
	else
		return true;
}


DCF77Decoder::DCF77Decoder() {
	// Empty constructor - unused
}


bool DCF77Decoder::set(bool data[59]) {
	// First bit must be 0
	if (data[0] != 0) return false;
	
	/*
	 * Meteo data can't be decoded, so it's skipped
	 */
	 
	// Antenna (or TX status); 0 - primary, 1 - secondary (or maintenance mode)
	result.antenna = data[15];
	
	// True when next hour is hour change
	result.timeChange = data[16];
	
	// When 1, it's summer time (CEST), otherwise it's winter time (CET)
	result.summerTime = bcd2dec(0, 0, data[18], data[17]) - 1;
	
	// 20nd bit must be 1, it means start time data
	if (data[20] != 1) return false;
	
	// Get minutes and check parity bit
	result.time.minute = bcd2dec(data[24], data[23], data[22], data[21])
					   + (bcd2dec(0, data[27], data[26], data[25]) * 10);
					   
	if (parityBit(data, 21, 27) != data[28]) return false;
	
	// Get hour
	result.time.hour = bcd2dec(data[32], data[31], data[30], data[29]) 
					 + (bcd2dec(0, 0, data[34], data[33]) * 10);
		 
	if (parityBit(data, 29, 34) != data[35]) return false;
	
	// Get day of month
	result.time.day = bcd2dec(data[39], data[38], data[37], data[36])
					+ (bcd2dec(0, 0, data[41], data[40]) * 10);
					  
	// Weekday (1 - monday, 7 - sunday)
	result.time.weekday = bcd2dec(0, data[44], data[43], data[42]);
	
	// Month (1 - jan, 12 - dec)
	result.time.month = bcd2dec(data[48], data[47], data[46], data[45])
					  + (bcd2dec(0, 0, 0, data[49]) * 10);
					  
	// Two digits of year in current century (example (19)96, (20)14)
	result.time.year = bcd2dec(data[53], data[52], data[51], data[50])
					 + (bcd2dec(data[57], data[56], data[55], data[54]) * 10);
							 
	// Parity bit for date
	if (parityBit(data, 36, 57) != data[58]) return false;
					 
	// Validate decoded data
	if ( (result.time.minute > 60) ||
		 (result.time.hour > 24) ||
		 (result.time.day > 31) ||
		 (result.time.weekday > 7) ||
		 (result.time.month > 12))
		return false;
	
	// Data correct - return true
	return true;
}


bool DCF77Decoder::set(unsigned short data[59]) {
	/*
	 * Overloaded function set(bool[59]) for int array
	 */
	bool newData[59];
	
	for (unsigned char i = 0; i <= 58; i++) {
		if (data[i] == 1)
			newData[i] = true;
		else
			newData[i] = false;
	}
	
	// Call original function
	return set(newData);
}


bool DCF77Decoder::set(std::string data) {
	/*
	 * Overloaded function set(bool[59]) for string
	 * String must have 58/59 characters
	 */
	bool newData[59];
	
	// Validate string length
	if (data.length() == 58) data = '0' + data;
	else if (data.length() != 59) return false;
	
	for (unsigned char i = 0; i <= 58; i++) {
		if (data[i] == '1')
			newData[i] = true;
		else
			newData[i] = false;
	}
	
	// Call original function
	return set(newData);
}


DCF77Result DCF77Decoder::getResult() {
	/*
	 * Just returns data...
	 */
	return result;
}

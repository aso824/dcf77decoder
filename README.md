DCF77Decoder
============

Data decoder class in C/C++. Decodes time and date from DCF77 transmitter.

Introduction
============

DCF77 is time transmitter with signal range over Europe, located in Germany. It works on 77.5kHz frequency with AM modulation and 50kW power (30-35kW EIRP). Frame is in digital format, it's 59 bit binary string and it's transmitted each minute. This class can decode such 59 bits and get all information except weather (it's first 14 bits and they are crypted with DES40).


Usage
=====

Data structs
------------

For definition look into `dcf77decoder.cpp` file.

Struct `DCF77Time` stores such informations (certain fields in this struct right below):

    struct DCF77Time {
	    unsigned short hour, minute, day, weekday, month, year;
    }
    
Please note that `year` variable is last two digits, for example `96` means 1996. 

Struct `DCF77Result` is designed to handle returned data from decoder class. Fields:

    struct DCF77Result {
      DCF77Time time;
      bool antenna, timeChange, summerTime;
    }
    
Usage
-----

For example, please open `example.cpp` file and look into `main()` function.


**Set functions**

To load data, use DCF77Decoder::set(...):

  - set(bool data[59])
  - set(unsigned short data[59])
  - set(std::string data)

After it, you can use function `DCF77Result getResult()`.
Example usage:

    std::string data = "00011110001100100010110001000010010010010000110000001010000";
    DCF77Decoder decoder;
    DCF77Result result;
    if (!decoder.set(data))
    	std::cerr << "Invalid data";
    else
    	result = decoder.getResult();
    std::cout << "It's " << result.time.hour << ":" << result.time.minute << std::endl;

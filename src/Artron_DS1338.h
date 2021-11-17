#ifndef __ARTRON_DS1338_H__
#define __ARTRON_DS1338_H__

#include "Arduino.h"
#include "Wire.h"
#include <time.h>

class Artron_DS1338 {
    private:
        TwoWire *wire = NULL;

        uint8_t BCDtoDEC(uint8_t n) ;
        uint8_t DECtoBCD(uint8_t n) ;

    public:
        Artron_DS1338(TwoWire *bus = &Wire);
        
        bool begin() ;
        bool read(struct tm *timeinfo) ;
        bool write(struct tm *timeinfo) ;

        bool set(int hour, int minute, int second, int mday, int month, int year) ;
        bool get(int *hour, int *minute, int *second, int *mday, int *month, int *year) ;

        bool setTime(int hour, int minute, int second) ;
        bool setDate(int mday, int month, int year) ;

        bool getTime(int *hour, int *minute, int *second) ;
        bool getDate(int *mday, int *month, int *year) ;

};

#endif


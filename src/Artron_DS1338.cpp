#include "Artron_DS1338.h"

#define DS1338_ADDR 0x68

Artron_DS1338::Artron_DS1338(TwoWire *bus) {
    this->wire = bus;
}

bool Artron_DS1338::begin() {
    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0x07); // Start at address 0x07
    this->wire->write(0); // Write 0 to Oscillator Stop Flag for start
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    return true;
}

bool Artron_DS1338::read(struct tm* timeinfo) {
    if (!timeinfo) {
        return false;
    }

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0); // Start at address 0
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    size_t len = this->wire->requestFrom(DS1338_ADDR, 7);
    if (len != 7) {
        return false;
    }

    uint8_t buff[7];
    this->wire->readBytes(buff, 7);

    timeinfo->tm_sec = BCDtoDEC(buff[0] & 0x7F);
    timeinfo->tm_min = BCDtoDEC(buff[1] & 0x7F);
    timeinfo->tm_hour = BCDtoDEC(buff[2] & 0x3F);
    timeinfo->tm_wday = BCDtoDEC(buff[3] & 0x07);
    timeinfo->tm_mday = BCDtoDEC(buff[4] & 0x3F);
    timeinfo->tm_mon = BCDtoDEC(buff[5] & 0x1F) - 1;
    timeinfo->tm_year = BCDtoDEC(buff[6]);// + 2000 - 1900;

    return true;
}

bool Artron_DS1338::write(struct tm* timeinfo) {
    uint8_t buff[7];
    buff[0] = DECtoBCD(timeinfo->tm_sec) & 0x7F;
    buff[1] = DECtoBCD(timeinfo->tm_min) & 0x7F;
    buff[2] = DECtoBCD(timeinfo->tm_hour) & 0x3F;
    buff[3] = DECtoBCD(timeinfo->tm_wday) & 0x07;
    buff[4] = DECtoBCD(timeinfo->tm_mday) & 0x3F;
    buff[5] = DECtoBCD(timeinfo->tm_mon + 1) & 0x1F;
    buff[6] = DECtoBCD((timeinfo->tm_year + 1900) % 100);

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0); // Start at address 0
    this->wire->write(buff, 7);
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    return true;
}

uint8_t Artron_DS1338::BCDtoDEC(uint8_t n) {
    return ((n >> 4)  * 10) + (n & 0x0F);
}

uint8_t Artron_DS1338::DECtoBCD(uint8_t n) {
    return (((n / 10) << 4) & 0xF0) | ((uint8_t)(n % 10) & 0x0F);
}

// ---------------
bool Artron_DS1338::set(int hour, int minute, int second, int mday, int month, int year) {
    struct tm timeinfo;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    timeinfo.tm_mday = mday;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_year = year - 1900;

    return write(&timeinfo);
}

bool Artron_DS1338::get(int *hour, int *minute, int *second, int *mday, int *month, int *year) {
    struct tm timeinfo;
    if (!read(&timeinfo)) {
        return false;
    }

    *hour = timeinfo.tm_hour;
    *minute = timeinfo.tm_min;
    *second = timeinfo.tm_sec;
    *mday = timeinfo.tm_mday;
    *month = timeinfo.tm_mon + 1;
    *year = timeinfo.tm_year + 1900;

    return true;
}


bool Artron_DS1338::setTime(int hour, int minute, int second) {
    uint8_t buff[3];
    buff[0] = DECtoBCD(second) & 0x7F;
    buff[1] = DECtoBCD(minute) & 0x7F;
    buff[2] = DECtoBCD(hour) & 0x3F;

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0); // Start at address 0
    this->wire->write(buff, 3);
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    return true;
}

bool Artron_DS1338::setDate(int mday, int month, int year) {
    uint8_t buff[3];
    buff[0] = DECtoBCD(mday) & 0x3F;
    buff[1] = DECtoBCD(month) & 0x1F;
    buff[2] = DECtoBCD(year % 100);

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0x04); // Start at address 04H
    this->wire->write(buff, 3);
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    return true;
}

bool Artron_DS1338::getTime(int *hour, int *minute, int *second) {
    if ((!hour) || (!minute) || (!second)) {
        return false;
    }

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0); // Start at address 0
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    size_t len = this->wire->requestFrom(DS1338_ADDR, 3);
    if (len != 3) {
        return false;
    }

    uint8_t buff[3];
    this->wire->readBytes(buff, 3);

    *second = BCDtoDEC(buff[0] & 0x7F);
    *minute = BCDtoDEC(buff[1] & 0x7F);
    *hour = BCDtoDEC(buff[2] & 0x3F);

    return true;
}

bool Artron_DS1338::getDate(int *mday, int *month, int *year) {
    if ((!year) || (!month) || (!mday)) {
        return false;
    }

    this->wire->beginTransmission(DS1338_ADDR);
    this->wire->write(0x04); // Start at address 04H
    if (this->wire->endTransmission() != 0) {
        return false;
    }

    size_t len = this->wire->requestFrom(DS1338_ADDR, 3);
    if (len != 3) {
        return false;
    }

    uint8_t buff[3];
    this->wire->readBytes(buff, 3);

    *mday = BCDtoDEC(buff[0] & 0x3F);
    *month = BCDtoDEC(buff[1] & 0x1F);
    *year = BCDtoDEC(buff[2]) + 2000;

    return true;
}

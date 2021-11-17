#include <Arduino.h>
#include "Wire.h"
#include "Artron_DS1338.h"

Artron_DS1338 rtc;

void setup() {
  Serial.begin(115200);

  Wire.begin(); // Setup I2C to work
  rtc.begin(); // Enable DS1338 clock run

  if (!rtc.set(22, 18, 0, 17, 11, 2021)) { // Set Datetime to DS1338
    Serial.println("RTC set datetime fail"); // Print error if set fail
  }
}

void loop() {
  int hour, minute, second, mday, month, year = 0;

  if (rtc.get(&hour, &minute, &second, &mday, &month, &year)) { // Get Datetime from DS1338 then put to variable
    Serial.printf("Datetime: %d:%02d:%02d %d/%d/%d\n", // Print datetime to Serial Monitor
      hour, minute, second,
      mday, month, year
    );
  } else {
    Serial.println("RTC get datetime fail"); // Print error if read fail
  }

  delay(1000);
}

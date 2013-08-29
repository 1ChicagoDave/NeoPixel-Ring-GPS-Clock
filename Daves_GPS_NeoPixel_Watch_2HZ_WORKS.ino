#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>


#define PMTK_SET_NMEA_UPDATE_5_SEC  "$PMTK220,5000*1B"
#define PMTK_SET_NMEA_UPDATE_2HZ  "$PMTK220,500*2B"
#define PMTK_SET_NMEA_BAUD_RATE_115200  "$PMTK251,115200*1D"

//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop 
// and help support open source hardware & software! -ada


// If you're using a GPS module:
// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// If using software serial (sketch example default):
//   Connect the GPS TX (transmit) pin to Digital 8
//   Connect the GPS RX (receive) pin to Digital 7
// If using hardware serial (e.g. Arduino Mega):
//   Connect the GPS TX (transmit) pin to Arduino RX1, RX2 or RX3
//   Connect the GPS RX (receive) pin to matching TX1, TX2 or TX3

// If you're using the Adafruit GPS shield,  
//  -> SoftwareSerial mySerial(8, 7);
// and make sure the switch is set to SoftSerial

// If using software serial, keep these lines enabled
// (you can change the pin numbers to match your wiring):
SoftwareSerial mySerial(8, 7);    // OKAY!
Adafruit_GPS GPS(&mySerial);    // Initialize GPS w/SoftwareSerial

// If using hardware serial (e.g. Arduino Mega), comment
// out the above two lines and enable this line instead:
//HardwareSerial* serial1 = new HardwareSerial();

#define PIN 6  // Pin NeoPixel Ring is on
// You can set this to whichever digital pin is convenient for you

//  Initialize NeoPixels!  -- How many LEDS....etc...etc....
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console  -- FASTER CODE
// Set to 'true' if you want to debug and listen to the raw GPS sentences. -- SLOWER CODE  
// *** 'true' may interfere with seconds display accuracy
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// Set clock variables - 
// set your timezone
// EX:  (ET = -4), (CST = -5), (MT = -6), (PST = -7)
const int TimeZone = -5;    // (CST = UTC - 5)

// Which LEDS are what part of clock face -
const int clock[13][2]= {
  {
    0,15                        }  // 0 / 12
  ,
  {
    14,14                        }  // 1
  ,
  {
    13,13                        }  // 2
  ,
  {
    12,11                        }  // 3
  ,
  {
    10,10                        }  // 4
  ,
  {
    9,9                        }  // 5
  ,
  {
    8,7                        }  // 6
  ,
  {
    6,6                        }  // 7
  ,
  {
    5,5                        }  // 8
  ,
  {
    4,3                        }  // 9
  ,
  {
    2,2                        }  // 10
  ,
  {
    1,1                        }  // 11
  ,
  {
    0,15                        }  // 12
    
};

void setup()  
{

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out    -  Leave commented out to ensure better/faster timing of clock & display
 // Serial.begin(115200);
  // Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  delay(100);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_2HZ);   // 2 Hz update rate
  // Helps to parse fast enough to flash secondPixel EVERY SECOND!
  delay(100);

  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
  // GPS.sendCommand(PMTK_SET_NMEA_BAUD_RATE_115200);  // Set Baud Rate 115200 --> Faster - better??
  // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);
delay(100);
  GPS.begin(9600);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);

  // Ask for firmware version
  //  mySerial.println(PMTK_Q_RELEASE);

  strip.begin();
  delay(1000);
  // strip.show(); // Initialize all pixels to 'off'
  //    Runs through colors - "Se;f Test / Initialization"
  //colorWipe(255, 20);    // GREEN
  //colorWipe(0xFFF000, 20);      // YELLOW
  //colorWipe(0x0000FF, 20);        // BLUE
  //colorWipe(0x000000, 20);          // CLEAR

}    // End void setup()


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) 
{
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
  // writing direct to UDR0 is much much faster than Serial.print 
  // but only one character can be written at a time. 
#endif
}    // End SIGNAL(TIMER0...


void useInterrupt(boolean v) 
{
  if (v) 
  {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } 
  else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}


uint32_t timer = millis();

void loop()                     // run over and over again
{
  
  GPS.begin(9600);
  delay(20);
  GPS.read();
  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  //if (! usingInterrupt) {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  //  }

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) 
  {

    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    // Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 1 seconds, print out the current stats
  // also helps appearance of second pixel flashing every second
  if (millis() - timer > 1000) 
  { 
    timer = millis(); // reset the timer

    GPS.read();
    /*
    // Uncomment if you want to watch serial monitor
     
     Serial.print("\nTime: ");
     Serial.print(GPS.hour + 19, DEC); // TimeZone change!
     Serial.print(':');
     Serial.print(GPS.minute, DEC); 
     Serial.print(':');
     Serial.print(GPS.seconds, DEC); 
     Serial.print('.');
     Serial.println(GPS.milliseconds);
     */


    // Set Arduino clock using the time.h library
    setTimeClock();
    delay(10);
    // turn off serial communication to free up timer for NeoPixel clock display
  //  mySerial.end();
    // We turn it back on at beginning of loop again
delay(30);
    // Uncomment following lines for more "feedback" and debugging, etc...
    // initNeoPixels();
    digitalClockDisplay();

    setSecond();
    setBackground();
    setHour();
    setMinute();

  }

}

void setTimeClock()
{
  int clockHour = 12;
  // adjust for timezone setting
  if (GPS.hour <= abs(TimeZone))
  {
    clockHour = ((GPS.hour + (12 + TimeZone)));
  }
  else if ((GPS.hour > abs(TimeZone)) && (GPS.hour <= (12 + abs(TimeZone))))
  {
    clockHour = (GPS.hour + TimeZone);
  }
  else if (GPS.hour > (12 + abs(TimeZone)))
  {
    clockHour = (GPS.hour - (12 + abs(TimeZone)));
  }
  // use time.h lbrary to set Arduino's timeclock
  setTime(clockHour , GPS.minute, GPS.seconds, GPS.day, GPS.month, GPS.year);
  delay(10); 
}

void setBackground()
{
  // Set the Clock Face" however we feel. I used two YELLOW lights to mark 12 o'clock for simplicity & personal taste
  // I left in commented code for 3,6, & 9 o'clock dial positions for you
  for(uint16_t i=0; i<16; i++) 
  {
    // Start with a clean slate!
    strip.setPixelColor(i, 0,0,0);  // sets background to blank - clears all LEDs
  }
  // twelve o'clock position
  strip.setPixelColor((clock[12][0]),35,35,0);
  strip.setPixelColor((clock[12][1]),35,35,0);

  // three o'clock position
  // strip.setPixelColor((clock[3][0]),35,35,0);
  // strip.setPixelColor((clock[3][1]),35,35,0);

  // six o'clock position
  // strip.setPixelColor((clock[6][0]),35,35,0);
  // strip.setPixelColor((clock[6][1]),35,35,0);

  // nine o'clock position
  //  strip.setPixelColor((clock[9][0]),15,15,0);
  //  strip.setPixelColor((clock[9][1]),15,15,0);

  strip.show();  // Light 'em up!
  delay(10);
}  // end setBackground

void setHour()
{

  strip.setPixelColor((clock[hour()][0]), 0,0,25);
  strip.setPixelColor((clock[hour()][1]), 0,0,25);

  strip.show();
  delay(10);
}  // end setHour()

void setMinute()
{
  // convert 60 possible minutes to groups of five for our usage - (Relies on fabulous integer math!)
  int clockMinute = minute();
  int minuteDot = (clockMinute / 5);

  // blink Minute dot number of minutes out of five minute "group"
  // blink 0 times on the "zero" - ( 0, 5, 10, 15, 20, etc...)
  // blinks once on the "ones" - ( 1, 6, 11, 16, 21, etc....)
  // blinks twice on the "twos" - ( 2, 7, 12, 17, 22, etc....)
  // See a pattern here??   :)
  if ((minute()%5) !=0)
  {
    for ( int i = 1; i <= ((minute() %5 )); i++)
    {
      // change color to whatever you'd like!
      // I'm using a dim green here
      strip.setPixelColor((clock[minuteDot][0]), 0,75,0);
      strip.setPixelColor((clock[minuteDot][1]), 0,75,0);
      strip.show();
      delay(40);
      // and clear!
      strip.setPixelColor((clock[minuteDot][0]), 0,0,0);
      strip.setPixelColor((clock[minuteDot][1]), 0,0,0);
      strip.show();
      delay(40);
    }
  }
  // Then...after the blinks to indicate exact minutes, this "holds" the light steady until the next second-cycle
  // I've kept this color the same as the one above...slightly dimmer.
  strip.setPixelColor((clock[minuteDot][0]), 0,25,0);
  strip.setPixelColor((clock[minuteDot][1]), 0,25,0);
  strip.show();
  delay(10);


}  // end setMinute()

void setSecond()
{

  // more integer math to split 60 possible seconds into 12 possible dial locations
  int clockSecond = second();
  int secondDot = (clockSecond / 5);
  // Blink the secondPixel every second & Update seconds variable if necessary

  // Something I was playing with - it "races" the second indicator from 12 o'clock to the current position every second.
  // kind of neat, but "wastes time"   :)
  //"Race" to second position
  /* for ( int i = 1; i<= (secondDot); i++)
   {
   strip.setPixelColor((clock[i][0]), 55,0,0);
   strip.setPixelColor((clock[i][1]), 55,0,0);
   strip.show();
   delay(10);
   strip.setPixelColor((clock[i][0]), 0,0,0);
   strip.show();
   delay(10);
   
   }
   */

  // Blinks once every second (or so) to indcate times position within the current minute
  strip.setPixelColor((clock[secondDot][0]), 45,0,0);    // Turn on
  strip.setPixelColor((clock[secondDot][1]), 45,0,0);    // Turn on
  strip.show();
  delay(40);
 //  strip.setPixelColor((clock[secondDot][0]), 0,0,0);    // Turn off - only flash/blink in seconds position
 //  strip.setPixelColor((clock[secondDot][1]), 0,0,0);    // Turn off - only flash/blink in seconds position
  /// strip.show();
  //delay(10);

}  // end setSecond()



void initNeoPixels()
{

  // a simple colorWipe-type of indicator that we commented out way aboave
  for (int i =0; i < 16 ; i++)
  {
    strip.setPixelColor(i, 0,0,0);
    strip.show();
    delay(10);
    strip.setPixelColor(i, 55,0,5);
    strip.show();
  }    // end for 
}    // end initNeoPixels()

void digitalClockDisplay()
{
  // we commented this out above also. It writes the time to serial monitor for debugging, etc...

  // Turn on Serial...
  Serial.begin(115200);
  // digital clock display of the time
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print(" ");
  // ....and Date
  /*
  Serial.print(day());
   Serial.print(" ");
   Serial.print(month());
   Serial.print(" ");
   Serial.print(year()); 
   Serial.print(); 
   */
  // Carriage return / newline
  Serial.println(" ");
  // turn Serial off again... so timers work correctly!
  Serial.end();

}


//    ** BORRROWED from ADAFRUIT EXAMPLE CODE!!
void colorWipe(uint32_t c, uint8_t wait) 
{
  for( uint16_t i=0; i<16; i++) 
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}











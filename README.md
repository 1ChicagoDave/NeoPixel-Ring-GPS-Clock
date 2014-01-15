NeoPixel-Ring-GPS-Clock
=======================

Arduino "sketch" code for a simple LED clock using [Adafruit GPS Shield](http://www.adafruit.com/products/1272) \& [NeoPixel Ring](http://www.adafruit.com/products/1463).   

_Click on inline links for product-specific information \& support._      
     
This sketch was written for use with the [ChronoDot RTC](http://docs.macetech.com/doku.php/chronodot) from [MaceTech](http://www.macetech.com/) which uses the [DS3231DS](http://datasheets.maximintegrated.com/en/ds/DS3231.pdf) "Extremely Accurate I2C-Integrated RTC/TCXO/Crystal". According to MaceTech -

> The I2C interface is very straightforward and virtually identical to the register addresses of the popular DS1337 and DS1307 RTCs, which means that existing code for the Arduino, Basic Stamp, Cubloc, and other controllers should work with no modification.     

To see the register address table for ChronoDot's DS3231 RTC chip, [click here](https://github.com/1ChicagoDave/NeoPixel-Ring-GPS-Clock/wiki/DS3231DS-Timekeeping-Registers)     

For help using the Ultimate GPS Shield, Module, or Breakout, follow Adafruit's helpful [tutorial](http://learn.adafruit.com/adafruit-ultimate-gps-logger-shield). 

The latest library and example code for running the NeoPixel Ring can be found [here](https://github.com/adafruit/Adafruit_NeoPixel). If you are new to NeoPixels _\(WS2812 RGB LEDs)_ there is a lot of great information in Adafruit's [NeoPixel Überguide](http://learn.adafruit.com/adafruit-neopixel-uberguide).   



<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work by <a xmlns:cc="http://creativecommons.org/ns#" href="https://github.com/1ChicagoDave" property="cc:attributionName" rel="cc:attributionURL">David R Ratliff</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.<br />Based on a work at <a xmlns:dct="http://purl.org/dc/terms/" href="https://github.com/1ChicagoDave" rel="dct:source">https://github.com/1ChicagoDave</a>.

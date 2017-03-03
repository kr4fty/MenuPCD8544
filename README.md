Fork of the library made by oryshed (oryshed dot gmail dot com)

**Original**: [http://www.oryshed.com/arduino/lcd-48x84-shield] (http://www.oryshed.com/arduino/lcd-48x84-shield)

Requires the [PCD8544 library](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library) (display handling) and the [AnalogKeyPad library](https://github.com/kr4fty/AnalogKeyPad) (key input for menu navigation) mentioned above (optional).

Advantages: the [Adafruit](https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library) library is used.
Disadvantage: setStatus() and clearStatus() are not available. But they can be implemented externally, as for example in this sketch: 
[PID-temperature-control](https://github.com/kr4fty/PID-temperature-control), see [menu.cpp](https://github.com/kr4fty/PID-temperature-control/blob/master/menu.cpp).

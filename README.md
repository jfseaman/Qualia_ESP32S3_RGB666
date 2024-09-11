A collection of "ports" of standard GFX Library examples for the Adafruit Qualia RGB666 ESP32-S3 board
and HD40015C40 4 inch round display.

Most are Arduino-IDE and associated library based. CircuitPython is included. Note: I am not
a pytyon programmer. I have been writing C code for over 40 years. I don't even take advantage
of c++ much.

There are notable differences from the "Example" programs most of these where created from. One 
is that for these types of programs, I have it so that Serial output waits for a connection from 

Notes on VSCode versions:
In platformio.ini you will need to fixup the lib_extra_dir to point to your Arduino IDE libaries location 
or add libraries to the local project folder.

Make your selection according to the table below

    | Arduino IDE Setting                  | Value                                |
    | ------------------------------------ | ------------------------------------ |
    | Board                                | **Adafruit Qualia ESP32-S3 RGB666**  |
    | Port                                 | Your port                            |
    | USB CDC On Boot                      | Enable                               |
    | CPU Frequency                        | 240MHZ(WiFi)                         |
    | Core Debug Level                     | None                                 |
    | USB DFU On Boot                      | Disable                              |
    | Erase All Flash Before Sketch Upload | Disable                              |
    | Events Run On                        | Core1                                |
    | Flash Mode                           | QIO 80MHZ                            |
    | Flash Size                           | **16MB(128Mb)**                      |
    | Arduino Runs On                      | Core1                                |
    | USB Firmware MSC On Boot             | Disable                              |
    | Partition Scheme                     | **Default(6.25MB APP/3.43MB FATFS)** |
    | PSRAM                                | **OPI PSRAM**                        |
    | Upload Mode                          | **UART0/Hardware CDC**               |
    | Upload Speed                         | 921600                               |
    | USB Mode                             | **CDC and JTAG**                     |
    * The options in bold are required, others are selected according to actual conditions.

    If you lose your ability to upload without power cycle+ boot0 then reload 
    the factory firmware using https://adafruit.github.io/Adafruit_WebSerial_ESPTool/
    and upload a shetch while still in bootsel mode. After that your ability to upload
    should be fixed.

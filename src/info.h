//
// To get a suitably high GSCLK frequency, the CLKO-FUSE of the ATMega168/328 has been programmed
// It will output it's system clock on PB0 ("digital pin" #8)
//
// THIS REQUIRES AN ISP PROGRAMMER (or 2nd Arduino loaded with Arduino-ISP)
//
// Diecimila + ATmega168: lfuse: 0xBF
//                        hfuse: 0xDD
//                        efuse: 0x00 (or 0xF8)
//
// Uno + ATmega328:       lfuse: 0xBF
//                        hfuse: 0xDE
//                        efuse: 0x05 (or 0xFD)
//                        
// To revert to Arduino's default FUSE settings, change 0xBF to 0xFF.
//
// Use: "http://www.engbedded.com/fusecalc" and ".../arduino-XXX/hardware/arduino/boards.txt"
//
// Make sure to get the latest MBI5043 datasheet (at least version Jan. 2009)!           
//                        
//                    
//                                                
//                                 +----------------------+
//                                 |                      |
//                                 |      +--------+      |
//                                 |      |        |      |
//                                 |      +--------+      |
//                                 |       LED-sel.       |
//                                 |                      |
//                                 |                      |
//                         GND O---O 1  GND      VCC   24 O---O 5V
//                                 |                      |
//    Arduino Uno pin #14 (A0) O---O 2  SDI      R-EXT 23 O
//                                 |                      |
//    Arduino Uno pin #15 (A1) O---O 3  DCLK     SDO   22 O---O Arduino Uno pin #17 (A3)
//                                 |                      |
//    Arduino Uno pin #16 (A2) O---O 4  LE       GCLK  21 O---O Arduino Uno pin #8 
//                                 |                      |
//                        +--->|---O 5  LED0     LED15 20 O---|<---+
//                        |        |                      |        | 
//                        o--->|---O 6  LED1     LED14 19 O---|<---o
//                        |        |                      |        | 
//                        o--->|---O 7  LED2     LED13 18 O---|<---o    N:  number of LEDs per channel
//                        |        |                      |        |    Vf: LED forward voltage 
//                        o--->|---O 8  LED3     LED12 17 O---|<---o         
//                        |        |                      |        |    V-LED: abs. max. 17V
//                        o--->|---O 9  LED4     LED11 16 O---|<---o
//                        |        |                      |        |
//                        o--->|---O 10 LED5     LED10 15 O---|<---o
//                        |        |                      |        |
//                        o--->|---O 11 LED6     LED9  14 O---|<---o
//                        |        |                      |        |
//                        o--->|---O 12 LED7     LED8  13 O---|<---o---O V-LED ( approx. N x Vf + 0.7V )
//                        |        |                      |        |
//                        |        +----------------------+        |
//                        |                                        |
//                        +----------------------------------------+
//
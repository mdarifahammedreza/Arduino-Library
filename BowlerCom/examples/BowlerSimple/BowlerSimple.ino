/**
 * Bowler Server, this is a basic bowler server
 * lib https://github.com/madhephaestus/BowlerCom
 */
 int baudrate = 9600;
#include <BowlerCom.h>
int txPin =11;
int rxPin =10;
BowlerCom com;
SoftwareSerial mySerial(rxPin,txPin); // RX, TX

void setup() {

  mySerial.begin(baudrate);
  Serial.begin(baudrate);
#  if  defined(__ARDUINO_ARC__)
  while (!Serial ||!mySerial );    // wait for the serial port to open
#endif
  //Pass the communications stream to Bowler Server
  com.begin(&Serial);
  //pass the printing stream to the printing interface
  startDebugPint(&mySerial);

  setPrintLevelErrorPrint();
}

void loop() {
  com.server();
}


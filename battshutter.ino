#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

const int ledPin = LED_BUILTIN;

void setup() {
  //Serial.begin(19200);

  //Serial.println("Modbus RTU Server LED");

  encoder_init();
  motor_init();
  // start the Modbus RTU server, with (slave) id 1
  if (!ModbusRTUServer.begin(1, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // configure the LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 1);
}

void loop() {
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();
  encoder_poll();

  // read the current value of the coil
  int coilValue = ModbusRTUServer.coilRead(0x00);

  if (coilValue) {
    // coil value set, turn LED on
    digitalWrite(ledPin, HIGH);
  } else {
    // coild value clear, turn LED off
    digitalWrite(ledPin, LOW);
  }
  
  motor_poll();
}

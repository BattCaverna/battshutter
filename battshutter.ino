#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
#include "motor.h"

const int ledPin = LED_BUILTIN;

#define UP_SWITCH   A2
#define DOWN_SWITCH A1

#define DE_RS485 A0
#define RE_RS485 A3

#define MODBUS_ON 1

void setup() {
  RS485.setPins(RS485_DEFAULT_TX_PIN, DE_RS485, RE_RS485);
  
#if !MODBUS_ON
  Serial.begin(9600);
  Serial.println("BattShutter start");
#endif

  encoder_init();
  motor_init();
  pinMode(UP_SWITCH, INPUT_PULLUP);
  pinMode(DOWN_SWITCH, INPUT_PULLUP);

#if MODBUS_ON
  // start the Modbus RTU server, with (slave) id 1
  if (!ModbusRTUServer.begin(1, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 1);
  pinMode(ledPin, OUTPUT);
#endif
}

void loop() {
  encoder_poll();
  if (digitalRead(UP_SWITCH) == 0)
    motor_poll(MS_UP);
  else if (digitalRead(DOWN_SWITCH) == 0)
    motor_poll(MS_DOWN);
  else
    motor_poll(MS_STOP);
    
#if MODBUS_ON
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();


  // read the current value of the coil
  int coilValue = ModbusRTUServer.coilRead(0x00);

  if (coilValue) {
    // coil value set, turn LED on
    digitalWrite(ledPin, HIGH);
  } else {
    // coild value clear, turn LED off
    digitalWrite(ledPin, LOW);
  }
#endif
}

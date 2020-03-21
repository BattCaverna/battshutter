#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
#include "motor.h"

#include "cfg.h"

void setup() {
  Serial.begin(MODBUS_BAUDRATE);
  uint8_t addr = addr_init();
#if !MODBUS_ON
  Serial.print("BattShutter start, address:");
  Serial.println(addr);
#endif

  encoder_init();
  motor_init();
  pinMode(UP_SWITCH, INPUT_PULLUP);
  pinMode(DOWN_SWITCH, INPUT_PULLUP);

#if MODBUS_ON
  RS485.setPins(RS485_DEFAULT_TX_PIN, DE_RS485, RE_RS485);
  // start the Modbus RTU server, with (slave) id 1
  if (!ModbusRTUServer.begin(addr, MODBUS_BAUDRATE)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // configure a single coil at address 0x00
  ModbusRTUServer.configureCoils(0x00, 1);
  pinMode(LED_BUILTIN, OUTPUT);
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
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    // coild value clear, turn LED off
    digitalWrite(LED_BUILTIN, LOW);
  }
#endif
}

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include "motor.h"
#include "switches.h"

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
  switches_init();

#if MODBUS_ON
  RS485.setPins(RS485_DEFAULT_TX_PIN, DE_RS485, RE_RS485);
  // start the Modbus RTU server, with (slave) address
  if (!ModbusRTUServer.begin(addr, MODBUS_BAUDRATE)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // configure builtin led
  ModbusRTUServer.configureCoils(LED_COIL_REG, 1);
  pinMode(LED_BUILTIN, OUTPUT);

  // configure registers
  ModbusRTUServer.configureHoldingRegisters(START_REG, HOLD_REG_CNT);
#endif
}

void loop() {
  int target = encoder_poll();
  bool upd_target = false;
  
  switch (switches_poll())
  {
    case S_UP:
      target += (ENC_HYST + 1);
      upd_target = true;
      break;
    case S_DOWN:
      target -= (ENC_HYST + 1);
      upd_target = true;
      break;
    default:
      break;
  }

  if (upd_target)
    ModbusRTUServer.holdingRegisterWrite(TARGET_HOLD_REG, target);

#if MODBUS_ON
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();

  ModbusRTUServer.holdingRegisterWrite(ENC_HOLD_REG, encoder_position());

  target = ModbusRTUServer.holdingRegisterRead(TARGET_HOLD_REG);
  

  // read the current value of the coil
  int coilValue = ModbusRTUServer.coilRead(LED_COIL_REG);

  if (coilValue) {
    // coil value set, turn LED on
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    // coild value clear, turn LED off
    digitalWrite(LED_BUILTIN, LOW);
  }
#endif

  int delta = target - encoder_position();
  
  if (delta > ENC_HYST)
    motor_poll(MS_UP);
  else if (delta < -ENC_HYST)
    motor_poll(MS_DOWN);
  else
    motor_poll(MS_STOP);
}

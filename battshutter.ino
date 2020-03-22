#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include "motor.h"
#include "switches.h"

#include "cfg.h"


unsigned target_pos;

void setup() {
  Serial.begin(MODBUS_BAUDRATE);
  uint8_t addr = addr_init();

#if !MODBUS_ON
  Serial.print("BattShutter start, address:");
  Serial.println(addr);
#endif

  encoder_init();
  target_pos = encoder_position();
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
  ModbusRTUServer.holdingRegisterWrite(TARGET_REG, target_pos);
#endif
}

void loop() {
  encoder_poll();

#if MODBUS_ON
  // poll for Modbus RTU requests
  ModbusRTUServer.poll();

  ModbusRTUServer.holdingRegisterWrite(ENC_STEP_REG, encoder_position_step());
  ModbusRTUServer.holdingRegisterWrite(ENC_MAX_STEP_REG, encoder_max());
  ModbusRTUServer.holdingRegisterWrite(ENC_POS_REG, encoder_position());
  
  target_pos = ModbusRTUServer.holdingRegisterRead(TARGET_REG);


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

  Switches sw = switches_poll();
  if (sw != S_STOP)
  {
    target_pos = encoder_position();
#if MODBUS_ON
    ModbusRTUServer.holdingRegisterWrite(TARGET_REG, target_pos);
#endif
  }
  
  if (sw == S_RESET_ENC)
    encoder_reset();

  int delta = target_pos - encoder_position();

  if (delta > ENC_HYST || sw == S_UP)
    motor_poll(MS_UP);
  else if (delta < -ENC_HYST || sw == S_DOWN)
    motor_poll(MS_DOWN);
  else
    motor_poll(MS_STOP);
}

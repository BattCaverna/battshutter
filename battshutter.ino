#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include "motor.h"
#include "switches.h"

#include "cfg.h"


unsigned target_pos;
long last_modbus_poll;

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
    last_modbus_poll = millis();
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
  ModbusRTUServer.holdingRegisterWrite(ENC_STEP_REG, encoder_position_step());
  ModbusRTUServer.holdingRegisterWrite(ENC_MAX_STEP_REG, encoder_max());
  ModbusRTUServer.holdingRegisterWrite(ENC_POS_REG, encoder_position());


  // Poll modbus only once every MODBUS_POLL_TIME ms, otherwise 
  // we may reply too fast. The master needs some time (few ms)
  // to switch off the TX and turn the RX on.
  if (millis() - last_modbus_poll > MODBUS_POLL_TIME)
  {
    ModbusRTUServer.poll();
    last_modbus_poll = millis();
  }


  target_pos = ModbusRTUServer.holdingRegisterRead(TARGET_REG);
  if (target_pos < 0)
    target_pos = 0;

  if (target_pos > 100)
    target_pos = 100;

  // Rewrite clamped target
  ModbusRTUServer.holdingRegisterWrite(TARGET_REG, target_pos);

  int val = ModbusRTUServer.holdingRegisterRead(ENC_MAX_STEP_REG);
  encoder_setMax(val);

  val = ModbusRTUServer.holdingRegisterRead(ENC_STEP_REG);
  encoder_setCurr(val);


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
  else if (delta < -ENC_HYST || sw == S_DOWN || sw == S_RESET_ENC)
    motor_poll(MS_DOWN);
  else
    motor_poll(MS_STOP);
}

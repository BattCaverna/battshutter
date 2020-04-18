#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

#include "motor.h"
#include "switches.h"

#include "cfg.h"


unsigned target_pos;
Switches last_sw;

void setup() {
  Serial.begin(MODBUS_BAUDRATE);
  uint8_t addr = addr_init();

#if !MODBUS_ON
  Serial.print("BattShutter start, address:");
  Serial.println(addr);
#endif

  cfg_init();
  encoder_init();
  target_pos = encoder_position();
  motor_init();
  switches_init();
  last_sw = switches_poll();

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
  ModbusRTUServer.holdingRegisterWrite(MANUAL_SWITCH_REG, S_STOP);
#endif
}

void loop() {
  encoder_poll();
  cfg_update();

#if MODBUS_ON
  ModbusRTUServer.holdingRegisterWrite(ENC_STEP_REG, encoder_position_step());
  ModbusRTUServer.holdingRegisterWrite(ENC_MAX_STEP_REG, encoder_max());

  ModbusRTUServer.holdingRegisterWrite(CURR_POS_REG, encoder_position());

  ModbusRTUServer.holdingRegisterWrite(MOTOR_TIMEOUT_REG, motor_timeoutStatus());
  int motor_pos = (motor_position_time() + 50) / 100;
  ModbusRTUServer.holdingRegisterWrite(MOTOR_POS_REG, motor_pos);
  int motor_max_pos = (motor_max() + 50) / 100;
  ModbusRTUServer.holdingRegisterWrite(MOTOR_MAX_REG, motor_max_pos);

  ModbusRTUServer.poll();

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

  val = ModbusRTUServer.holdingRegisterRead(MOTOR_MAX_REG);
  if (val != motor_max_pos)
    motor_setMax(val * 100L);

  val = ModbusRTUServer.holdingRegisterRead(MOTOR_POS_REG);
  if (val != motor_pos)
    motor_setCurr(val * 100L);

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

#if MODBUS_ON

  // Only consider modbus switches if there aren't any manual ones
  if (sw == last_sw && sw == S_STOP)
  {
    sw = (Switches)ModbusRTUServer.holdingRegisterRead(MANUAL_SWITCH_REG);

    if (sw < S_STOP)
      sw = S_STOP;

    if (sw >= S_CNT)
      sw = (Switches)(S_CNT - 1);
  }
  else
    last_sw = sw;

  ModbusRTUServer.holdingRegisterWrite(MANUAL_SWITCH_REG, (int)sw);
#endif


  if (sw != S_STOP)
  {
    target_pos = encoder_position();
#if MODBUS_ON
    ModbusRTUServer.holdingRegisterWrite(TARGET_REG, target_pos);
#endif
  }

  if (sw == S_RESET_ENC)
  {
    encoder_reset();
    motor_scheduleCalibration();
  }

  int delta = target_pos - encoder_position();

  if (delta > ENC_HYST || sw == S_UP)
    motor_poll(MS_UP);
  else if (delta < -ENC_HYST || sw == S_DOWN || sw == S_RESET_ENC)
    motor_poll(MS_DOWN);
  else
    motor_poll(MS_STOP);
}

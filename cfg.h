#ifndef CFG_H
#define CFG_H

#define MODBUS_ON 1
#define MODBUS_BAUDRATE 9600

//Modbus address
#define ADDR0 3 // D3
#define ADDR1 4 // D3
#define ADDR2 5 // D3
#define BASE_ADDR 0x10 // Mobbus start address when pin address=0

#define LED_COIL_REG 0x00

// Modbus holding registers
#define START_REG 0x00

enum HoldingRegs
{
  ENC_HOLD_REG = START_REG, // Encoder
  TARGET_HOLD_REG, // Target position
  HOLD_REG_END
};

#define HOLD_REG_CNT (HOLD_REG_END - START_REG)

// Switches
#define UP_SWITCH   A2
#define DOWN_SWITCH A1

// RS485
#define DE_RS485 A0
#define RE_RS485 A3

// Encoder
#define ENC_PIN 2 //D2
#define ENC_HYST 0

//Motor
#define MOTOR_PWR 9 // D9
#define MOTOR_DIR 8 // D8
#define GUARD_TIME 500 //ms


#endif

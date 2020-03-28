#ifndef CFG_H
#define CFG_H

#define DISABLE485 0
#define MODBUS_ON 1
#define MODBUS_BAUDRATE 9600
#define MODBUS_POLL_TIME 25 //ms

//Modbus address
#define ADDR0 3 // D3
#define ADDR1 4 // D4
#define ADDR2 5 // D5
#define BASE_ADDR 0x10 // Modbus start address when pin address=0

#define LED_COIL_REG 0x00

// Modbus holding registers
#define START_REG 0x00

enum HoldingRegs
{
  ENC_STEP_REG = START_REG, // Encoder: current position (steps)
  ENC_MAX_STEP_REG,         // Encoder: max position (steps)
  ENC_POS_REG ,             // Encoder: current position (%)
  TARGET_REG,               // Target position to reach (%)
  MANUAL_SWITCH_REG,        // Simulate switches through modbus
  MOTOR_POS_REG,            // Give information on motor position (based on timeout and direction)

  HOLD_REG_END
};

#define HOLD_REG_CNT (HOLD_REG_END - START_REG)

// Switches
#define UP_SWITCH   A2
#define DOWN_SWITCH A1
// If we keep pressing the down button when we reach home,
// an encoder reset is performed
#define RESET_ENC_TIME 10000 //ms

// RS485
#if DISABLE485
#define DE_RS485 6 // D6
#define RE_RS485 7 // D7
#else
#define DE_RS485 A0
#define RE_RS485 A3
#endif

// Encoder
#define ENC_PIN 2 //D2
// Position tolerance when reaching target (%)
#define ENC_HYST 0
// Max time between two encoder edges, used to detect when we reach the home position
#define MAX_ENC_PERIOD 2000 //ms
// Time interval between eeprom updates
#define EEP_SAVE_INTERVAL 5000 // ms


//Motor
#define MOTOR_PWR 9 // D9
#define MOTOR_DIR 8 // D8
// Time before switching on the mains when changing direction
#define GUARD_TIME 300 //ms
#define MOTOR_TIMEOUT 40 //s


#endif

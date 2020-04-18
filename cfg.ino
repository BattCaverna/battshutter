#include <EEPROM.h>

typedef struct EepromCfg
{
  uint16_t tag;
  uint8_t ver;
  int encoder_max_pos;
  int encoder_curr_pos;
  long motor_time;
  long motor_time_max;
} EepromCfg;

EepromCfg shutter_cfg;
static EepromCfg old_cfg;

#define EEPROM_START_ADDR 0
#define EEPROM_TAG 0x4953
#define EEPROM_VER 0

static void cfg_default(EepromCfg *cfg);
static void cfg_default(EepromCfg *cfg)
{
  cfg->tag = EEPROM_TAG;
  cfg->ver = EEPROM_VER;
  cfg->encoder_curr_pos = 0;
  cfg->encoder_max_pos = 1;
  cfg->motor_time = 0;
  cfg->motor_time_max = 30000; // 30s
}

static long eeprom_last_save = 0;

void cfg_init()
{
  EEPROM.get(EEPROM_START_ADDR, shutter_cfg);
  
  if (shutter_cfg.tag != EEPROM_TAG || shutter_cfg.ver != EEPROM_VER)
  {
    cfg_default(&shutter_cfg);
    cfg_update();
  }
  old_cfg = shutter_cfg;
}

void cfg_update()
{
  if (millis() - eeprom_last_save > EEP_SAVE_INTERVAL)
  {
    if (memcmp(&shutter_cfg, &old_cfg, sizeof(old_cfg)) != 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      EEPROM.put(EEPROM_START_ADDR, shutter_cfg);
      digitalWrite(LED_BUILTIN, LOW);
    }
    old_cfg = shutter_cfg;
    eeprom_last_save = millis();
  }
}

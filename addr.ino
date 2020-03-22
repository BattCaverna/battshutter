
uint8_t addr_init()
{
  uint8_t addr = 0;
  pinMode(ADDR0, INPUT_PULLUP);
  pinMode(ADDR1, INPUT_PULLUP);
  pinMode(ADDR2, INPUT_PULLUP);

  addr |= (digitalRead(ADDR2) ? 0 : 1);
  addr <<= 1;

  addr |= (digitalRead(ADDR1) ? 0 : 1);
  addr <<= 1;

  addr |= (digitalRead(ADDR0) ? 0 : 1);

  // Only 3 bits address
  addr &= 0x07;

  return BASE_ADDR + addr;
}

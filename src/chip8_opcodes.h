void op_00E0 (Chip8 *chip8);
void op_00EE(Chip8 *chip8);
void op_1nnn(Chip8 *chip8, uint16_t nnn);
void op_6xkk(Chip8 *chip8, uint8_t x, uint8_t kk);
void op_7xnn(Chip8 *chip8, uint8_t x, uint8_t kk);
void op_Annn(Chip8 *chip8, uint16_t nnn);
void op_Dxyn(Chip8 *chip8, uint8_t x, uint8_t y, uint8_t n);


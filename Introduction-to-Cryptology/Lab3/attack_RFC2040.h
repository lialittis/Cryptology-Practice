int oracle(buffer_t *encrypted, buffer_t *key);
int get_padding_position(buffer_t *encrypted, buffer_t *key);
void prepare(buffer_t *corrupted, buffer_t *encrypted, buffer_t *decrypted,
			 int known_positions);
uchar find_last_byte(buffer_t *corrupted, int pad_position, buffer_t *key);
void full_attack(buffer_t *decrypted, buffer_t *encrypted, buffer_t *key);

#ifndef AES_METHODS_H
#define AES_METHODS_H

#include <stdint.h>
#include <stdio.h>

#define PRINTSTATE(state) \
	for (int i = 0; i < 4; i++) \
		for (int j = 0; j < 4; j++) \
			printf("%02x", state[j][i]);

int Nr, Nk, Nb;
void setNr(int value);
void setNk(int value);
void setNb(int value);

typedef uint8_t byte;
typedef uint32_t word;

void cipher(byte in[16], byte out[16], word expandedKey[Nb*(Nr+1)]);

void keyExpansion(byte key[4*Nk], word expandedKey[Nb*(Nr+1)]);
void addRoundKey(word expandedKey[Nb*(Nr+1)], byte state[4][4], int round);
word rotWord(word input);
byte subByte(byte input, byte box[16][16]);
word subWord(word input);

void mixColumns(byte state[4][4]);
byte ffAdd(byte a, byte b);
byte xtime(byte a);
byte ffMultiply(byte a, byte b);

void shiftRow(byte row[4], int times);
void shiftRows(byte state[4][4]);
void subBytes(byte state[4][4]);

void invCipher(byte in[16], byte out[16], word expandedKey[Nb*(Nr+1)]);
void invMixColumns(byte state[4][4]);
void invShiftRows(byte state[4][4]);
void invSubBytes(byte state[4][4]);

#endif

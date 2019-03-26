#include "aesMethods.h"

byte Sbox[0x10][0x10] = {
    	{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 } ,
    	{ 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 } ,
    	{ 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15 } ,
    	{ 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 } ,
    	{ 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 } ,
    	{ 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf } ,
    	{ 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 } ,
    	{ 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 } ,
    	{ 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 } ,
    	{ 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb } ,
    	{ 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 } ,
    	{ 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 } ,
    	{ 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a } ,
    	{ 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e } ,
    	{ 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf } ,
    	{ 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 } };
byte InvSbox[16][16] = {
	{ 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb } ,
	{ 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb } ,
	{ 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e } ,
	{ 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25 } ,
	{ 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92 } ,
	{ 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84 } ,
	{ 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06 } ,
	{ 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b } ,
	{ 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73 } ,
	{ 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e } ,
	{ 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b } ,
	{ 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4 } ,
	{ 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f } ,
	{ 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef } ,
	{ 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61 } ,
	{ 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d } };
word Rcon[52] = { 0x00000000, 
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 
        0x10000000, 0x20000000, 0x40000000, 0x80000000, 
        0x1B000000, 0x36000000, 0x6C000000, 0xD8000000, 
        0xAB000000, 0x4D000000, 0x9A000000, 0x2F000000, 
        0x5E000000, 0xBC000000, 0x63000000, 0xC6000000, 
        0x97000000, 0x35000000, 0x6A000000, 0xD4000000, 
        0xB3000000, 0x7D000000, 0xFA000000, 0xEF000000, 
        0xC5000000, 0x91000000, 0x39000000, 0x72000000, 
        0xE4000000, 0xD3000000, 0xBD000000, 0x61000000, 
        0xC2000000, 0x9F000000, 0x25000000, 0x4A000000, 
        0x94000000, 0x33000000, 0x66000000, 0xCC000000, 
        0x83000000, 0x1D000000, 0x3A000000, 0x74000000, 
        0xE8000000, 0xCB000000, 0x8D000000};

void setNr(int value) {
	Nr = value;
}
void setNk(int value) {
	Nk = value;
}
void setNb(int value) {
	Nb = value;
}

void cipher(byte in[16], byte out[16], word expandedKey[Nb*(Nr+1)]){
	byte state[4][Nb];
	int round = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < Nb; j++) {
			state[j][i] = in[i * 4 + j];
		}
	}
	printf("round[%02d].input    ", round);
	PRINTSTATE(state);
	printf("\n");

	addRoundKey(expandedKey, state, round);
	printf("round[%02d].k_sch    ", round);
	for(int i = 0; i < 4; i++)
		printf("%08x", expandedKey[round*4 + i]);
	printf("\n");

	for (round = 1; round < Nr; round++) {
		printf("round[%02d].start    ", round);
		PRINTSTATE(state);
		printf("\n");
		subBytes(state);
		printf("round[%02d].s_box    ", round);
		PRINTSTATE(state);
		printf("\n");
		shiftRows(state);
		printf("round[%02d].s_row    ", round);
		PRINTSTATE(state);
		printf("\n");
		mixColumns(state);
		printf("round[%02d].m_col    ", round);
		PRINTSTATE(state);
		printf("\n");
		addRoundKey(expandedKey, state, round);
		printf("round[%02d].k_sch    ", round);
		for(int i = 0; i < 4; i++)
			printf("%08x", expandedKey[round*4 + i]);
		printf("\n");
	}

	printf("round[%02d].start    ", round);
	PRINTSTATE(state);
	printf("\n");
	subBytes(state);
	printf("round[%02d].s_box    ", round);
	PRINTSTATE(state);
	printf("\n");
	shiftRows(state);
	printf("round[%02d].s_row    ", round);
	PRINTSTATE(state);
	printf("\n");
	addRoundKey(expandedKey, state, round);
	printf("round[%02d].k_sch    ", round);
	for(int i = 0; i < 4; i++)
		printf("%08x", expandedKey[round*4 + i]);
	printf("\n");

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < Nb; j++) {
			out[i * 4 + j] = state[j][i];
		}
	}
	printf("round[%02d].output   ", round);
	for (int i = 0; i < 16; i++)
		printf("%02x", out[i]);
	printf("\n");
}


void keyExpansion(byte key[4*Nk], word expandedKey[Nb*(Nr+1)]) {
	word temp;
	byte shift = 0x08;

	for (int i = 0; i < Nk; i++) {
		temp = 0x00;

		temp = temp | key[4*i];
		temp = (temp << shift) | key[4*i + 1];
		temp = (temp << shift) | key[4*i + 2];
		temp = (temp << shift) | key[4*i + 3];
		expandedKey[i] = temp;
	}

	for (int i = Nk; i < Nb*(Nr+1); i++) {
		temp = expandedKey[i - 1];
		if (i % Nk == 0) {
			temp = subWord(rotWord(temp)) ^ Rcon[i/Nk];
		} else if (Nk > 6 && i % Nk == 4) {
			temp = subWord(temp);
		}
		expandedKey[i] = expandedKey[i - Nk] ^ temp;
	}
}

void addRoundKey(word expandedKey[Nb*(Nr+1)], byte state[4][4], int round){
	word temp;
	byte shift = 0x08, mask = 0xff;

	for (int i = 0; i < 4; i++) {
		temp = 0x00 | state[0][i];
		temp = (temp << shift) | state[1][i];
		temp = (temp << shift) | state[2][i];
		temp = (temp << shift) | state[3][i];

		temp = temp ^ expandedKey[round * 4 + i];

		state[3][i] = temp & mask;
		state[2][i] = (temp >> shift) & mask;
		state[1][i] = (temp >> (shift * 2)) & mask;
		state[0][i] = (temp >> (shift * 3)) & mask;
	}
}

word rotWord(word input){
	word b0, b1, b2, b3;
	byte shift = 0x08, mask = 0xff;
	word output = 0x00;

	b3 = input & mask;
	b2 = (input >> shift) & mask;
	b1 = (input >> (shift * 2)) & mask;
	b0 = (input >> (shift * 3)) & mask;

	output = output | b1;
	output = (output << shift) | b2;
	output = (output << shift) | b3;
	output = (output << shift) | b0;
	
	return output;
}
byte subByte(byte input, byte box[16][16]){
	byte first, second;
	byte mask = 0x0f;

	first = (mask << 4) & input;
	second = mask & input;

	return box[(first >> 4)][second];
}
word subWord(word input){
	byte b0,b1,b2,b3;
	byte mask = 0xff, shift = 0x08;
	word output = 0x00;

	b3 = input & mask;
	b2 = (input >> shift) & mask;
	b1 = (input >> (shift * 2)) & mask;
	b0 = (input >> (shift * 3)) & mask;

	b3 = subByte(b3, Sbox);
	b2 = subByte(b2, Sbox);
	b1 = subByte(b1, Sbox);
	b0 = subByte(b0, Sbox);

	output = output | b0;
	output = (output << shift) | b1;
	output = (output << shift) | b2;
	output = (output << shift) | b3;

	return output;
}

void mixColumns(byte state[4][4]){
	byte new0i, new1i, new2i, new3i;

	for (int i = 0; i < 4; i++) {
		new0i = ffMultiply(state[0][i],0x02) ^ ffMultiply(state[1][i],0x03) ^ state[2][i] ^ state[3][i];
		new1i = state[0][i] ^ ffMultiply(state[1][i],0x02) ^ ffMultiply(state[2][i],0x03) ^ state[3][i];
		new2i = state[0][i] ^ state[1][i] ^ ffMultiply(state[2][i],0x02) ^ ffMultiply(state[3][i],0x03);
		new3i = ffMultiply(state[0][i],0x03) ^ state[1][i] ^ state[2][i] ^ ffMultiply(state[3][i],0x02);

		state[0][i] = new0i;
		state[1][i] = new1i;
		state[2][i] = new2i;
		state[3][i] = new3i;
	}
}
byte ffAdd(byte a, byte b){
	return a ^ b;
}

byte xtime(byte a){
	byte mask = 0x80;
	byte topBit = a & mask;
	a = a << 1;

	if (topBit) 
		a = a ^ 0x1b;

	return a;
}

byte ffMultiply(byte a, byte b){
	byte addCurrent;
	byte counter = 0x01, curResult = a, runTotal = 0x00;

	addCurrent = b & counter;
	if (addCurrent)
	       runTotal = ffAdd(runTotal, curResult);	
	counter = counter << 1;

	while (counter) {
		curResult = xtime(curResult);
		addCurrent = b & counter;
		if (addCurrent)
			runTotal = ffAdd(runTotal, curResult);
		counter = counter << 1;
	}

	return runTotal;
}

void shiftRow(byte row[4], int times){
	byte move;

	for (int i = 0; i < times; i++) {
		move = row[0];
		row[0] = row[1];
		row[1] = row[2];
		row[2] = row[3];
		row[3] = move;
	}
}

void shiftRows(byte state[4][4]){
	shiftRow(state[1], 1);
	shiftRow(state[2], 2);
	shiftRow(state[3], 3);
}

void subBytes(byte state[4][4]){
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			state[i][j] = subByte(state[i][j], Sbox);
}



void invCipher(byte in[16], byte out[16], word expandedKey[Nb*(Nr+1)]){
	byte state[4][Nb];
	int round = Nr;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < Nb; j++) {
			state[j][i] = in[i * 4 + j];
		}
	}

	printf("round[00].iinput   ");
	PRINTSTATE(state);
	printf("\n");
	

	addRoundKey(expandedKey, state, round);

	printf("round[%02d].ik_sch   ", Nr-round);
	for(int i = 0; i < 4; i++)
		printf("%08x", expandedKey[round*4+i]);
	printf("\n");

	for (round = Nr-1; round > 0; round--) {
		printf("round[%02d].istart   ", Nr-round);
		PRINTSTATE(state);
		printf("\n");
		invShiftRows(state);

		printf("round[%02d].is_row   ", Nr-round);
		PRINTSTATE(state);
		printf("\n");	
		invSubBytes(state);

		printf("round[%02d].is_box   ", Nr-round);
		PRINTSTATE(state);
		printf("\n");
		addRoundKey(expandedKey, state, round);

		printf("round[%02d].ik_sch   ", Nr-round);
		for(int i = 0; i < 4; i++)
			printf("%08x", expandedKey[round*4+i]);
		printf("\n");

		printf("round[%02d].ik_add   ", Nr-round);
		PRINTSTATE(state);
		printf("\n");
	
		invMixColumns(state);
	}
	
	printf("round[%02d].istart   ", Nr-round);
	PRINTSTATE(state);
	printf("\n");

	invShiftRows(state);
	printf("round[%02d].is_row   ", Nr-round);
	PRINTSTATE(state);
	printf("\n");
	
	invSubBytes(state);
	printf("round[%02d].is_box   ", Nr-round);
	PRINTSTATE(state);
	printf("\n");
	
	addRoundKey(expandedKey, state, round);
	printf("round[%02d].ik_sch   ", Nr-round);
	for(int i = 0; i < 4; i++)
		printf("%08x", expandedKey[round*4+i]);
	printf("\n");


	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < Nb; j++) {
			out[i * 4 + j] = state[j][i];
		}
	}
	printf("round[%02d].ioutput  ", Nr);
	for (int i = 0; i < 16; i++)
		printf("%02x", out[i]);
	printf("\n");
}

void invMixColumns(byte state[4][4]){
	byte new0i, new1i, new2i, new3i;

	for (int i = 0; i < 4; i++) {
		new0i = ffMultiply(0x0e, state[0][i]) ^ ffMultiply(0x0b, state[1][i]) ^ ffMultiply(0x0d, state[2][i]) ^ ffMultiply(0x09, state[3][i]);
		new1i = ffMultiply(0x09, state[0][i]) ^ ffMultiply(0x0e, state[1][i]) ^ ffMultiply(0x0b, state[2][i]) ^ ffMultiply(0x0d, state[3][i]);
		new2i = ffMultiply(0x0d, state[0][i]) ^ ffMultiply(0x09, state[1][i]) ^ ffMultiply(0x0e, state[2][i]) ^ ffMultiply(0x0b, state[3][i]);
		new3i = ffMultiply(0x0b, state[0][i]) ^ ffMultiply(0x0d, state[1][i]) ^ ffMultiply(0x09, state[2][i]) ^ ffMultiply(0x0e, state[3][i]);

		state[0][i] = new0i;
		state[1][i] = new1i;
		state[2][i] = new2i;
		state[3][i] = new3i;
	}
}

void invShiftRows(byte state[4][4]){
	shiftRow(state[1], 3);
	shiftRow(state[2], 2);
	shiftRow(state[3], 1);
}

void invSubBytes(byte state[4][4]){
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			state[i][j] = subByte(state[i][j], InvSbox);
}



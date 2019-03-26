#include "aesMethods.h"

int main(int argc, char* argv[]) {
	
	byte in[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	byte out[16];
	for (int i = 0; i < 16; i++)
		out[i] = 0x00;

	//AES128
	setNr(10);
	setNk(4);
	setNb(4);

	printf("\nC.1 AES-128 (Nk=%d, Nr=%d)\n", Nk, Nr);
	printf("PLAINTEXT:         00112233445566778899aabbccddeeff\n");
	printf("KEY:               000102030405060708090a0b0c0d0e0f\n");

	byte key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                          0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };	

	word key128[Nb*(Nr+1)];
	keyExpansion(key, key128);

	printf("\nCIPHER (ENCRYPT):\n");
	cipher(in, out, key128);
	printf("\nINVERSE CIPHER (DECRYPT):\n");
	invCipher(out, in, key128);

	//AES192
	setNr(12);
	setNk(6);

	printf("\n\nC.1 AES-192 (Nk=%d, Nr=%d)\n", Nk, Nr);
	printf("PLAINTEXT:         00112233445566778899aabbccddeeff\n");
	printf("KEY:               000102030405060708090a0b0c0d0e0f1011121314151617\n");

	byte key1[24] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };

	word key192[Nb*(Nr+1)];
	keyExpansion(key1, key192);

	printf("\nCIPHER (ENCRYPT):\n");
	cipher(in, out, key192);
	printf("\nINVERSE CIPHER (DECRYPT):\n");
	invCipher(out, in, key192);

	//AES256
	setNr(14);
	setNk(8);

	printf("\n\nC.1 AES-256 (Nk=%d, Nr=%d)\n", Nk, Nr);
	printf("PLAINTEXT:         00112233445566778899aabbccddeeff\n");
	printf("KEY:               000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f\n");

	byte key2[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
			0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
			0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

	word key256[Nb*(Nr+1)];
	keyExpansion(key2, key256);

	printf("\nCIPHER (ENCRYPT):\n");
	cipher(in, out, key256);
	printf("\nINVERSE CIPHER (DECRYPT):\n");
	invCipher(out, in, key256);

	return 0;
}

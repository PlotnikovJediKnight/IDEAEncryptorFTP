#include "IDEAEncryptor.h"

IDEAEncryptor* IDEAEncryptor::instance_pointer = nullptr;
IDEAEncryptor* IDEAEncryptor::getInstance() {
	if (instance_pointer == nullptr) {
		instance_pointer = new IDEAEncryptor;
	}
	return instance_pointer;
}

IDEAEncryptor::IDEAEncryptor() { 
	std::fill(c.ek, c.ek + IDEAKEYLEN, 0);
	std::fill(c.dk, c.dk + IDEAKEYLEN, 0);
}

void IDEAEncryptor::idea_key(byteptr key) {
	ideaExpandKey(key, c.ek);
	ideaInvertKey(c.ek, c.dk);
}

void IDEAEncryptor::ideaExpandKey(byteptr userkey, word16 EK[IDEAKEYLEN]) {
    int i, j;
    for (j = 0; j < 8; j++) {
        EK[j] = (userkey[0] << 8) + userkey[1];
        userkey += 2;
    }
    for (i = 0; j < IDEAKEYLEN; j++) {
        i++;
        EK[i + 7] = EK[i & 7] << 9 | EK[i + 1 & 7] >> 7;
        EK += i & 8;
        i &= 7;
    }
}

void IDEAEncryptor::ideaInvertKey(const word16 EK[IDEAKEYLEN], word16 DK[IDEAKEYLEN]) {
    int i;
    uint16 t1, t2, t3;
    word16 temp[IDEAKEYLEN];
    word16* p = temp + IDEAKEYLEN;
    t1 = mulInv(*EK++);
    t2 = -*EK++;
    t3 = -*EK++;
    *--p = mulInv(*EK++);
    *--p = t3;
    *--p = t2;
    *--p = t1;
    for (i = 0; i < IDEAROUNDS - 1; i++) {
        t1 = *EK++;
        *--p = *EK++;
        *--p = t1;
        t1 = mulInv(*EK++);
        t2 = -*EK++;
        t3 = -*EK++;
        *--p = mulInv(*EK++);
        *--p = t2;
        *--p = t3;
        *--p = t1;
    }
    t1 = *EK++;
    *--p = *EK++;
    *--p = t1;
    t1 = mulInv(*EK++);
    t2 = -*EK++;
    t3 = -*EK++;
    *--p = mulInv(*EK++);
    *--p = t3;
    *--p = t2;
    *--p = t1;

    memcpy(DK, temp, sizeof(temp));
    for (i = 0; i < IDEAKEYLEN; i++) temp[i] = 0;
}

uint16 IDEAEncryptor::mulInv(uint16 x) {
    uint16 t0, t1;
    uint16 q, y;
    if (x <= 1)
        return x;
    t1 = 0x10001L / x;
    y = 0x10001L % x;
    if (y == 1)
        return low16(1 - t1);
    t0 = 1;
    do {
        q = x / y;
        x = x % y;
        t0 += q * t1;
        if (x == 1)
            return t0;
        q = y / x;
        y = y % x;
        t1 += q * t0;
    } while (y != 1);
    return low16(1 - t1);
}

void IDEAEncryptor::idea_enc(byteptr data, int blocks, FILE* log) {
    int i;
    unsigned char* d = data;
    for (i = 0; i < blocks; i++) {
        if (log != nullptr) fprintf(log, "Block #%d\n", i + 1);
        ideaCipher(d, d, c.ek, log);
        d += 8;
    }
}

void IDEAEncryptor::idea_dec(byteptr data, int blocks, FILE* log) {
    int i;
    unsigned char* d = data;
    for (i = 0; i < blocks; i++) {
        if (log != nullptr) fprintf(log, "Block #%d\n", i + 1);
        ideaCipher(d, d, c.dk, log);
        d += 8;
    }
}

void IDEAEncryptor::ideaCipher(byteptr inbuf, byteptr outbuf, word16* key, FILE* log) {
    register uint16 x1, x2, x3, x4, s2, s3;
    word16* in, *out;

    register uint16 t16;
    register word32 t32;

    int r = IDEAROUNDS;
    in = (word16*)inbuf;
    x1 = *in++;
    x2 = *in++;
    x3 = *in++;
    x4 = *in;

    word16* key_p_out = key;

    if (log != nullptr) {
        fprintf(log, "Start of encryption process:\n");
        fprintf(log, "_______________________________________________________________________\n");
        fprintf(log, "|  K1  |  K2  |  K3  |  K4  |  K5  |  K6  ||  X1  |  X2  |  X3  |  X4  |\n");
        fprintf(log, "-----------------------------------------------------------------------\n");
        fprintf(log, "| ---- | ---- | ---- | ---- | ---- | ---- || %04x | %04x | %04x | %04x |\n", x1, x2, x3, x4);
    }

    do {
        key_p_out = key;
        MUL(x1, *key++);
        x2 += *key++;
        x3 += *key++;
        MUL(x4, *key++);
        s3 = x3;
        x3 ^= x1;
        MUL(x3, *key++);
        s2 = x2;
        x2 ^= x4;
        x2 += x3;
        MUL(x2, *key++);
        x3 += x2;
        x1 ^= x2; x4 ^= x3;
        x2 ^= s3; x3 ^= s2;
        if (log != nullptr)
            fprintf(log, "| %04x | %04x | %04x | %04x | %04x | %04x || %04x | %04x | %04x | %04x |\n", key_p_out[0], key_p_out[1], key_p_out[2], key_p_out[3], key_p_out[4], key_p_out[5], x1, x2, x3, x4);
    } while (--r);

    key_p_out = key;
    MUL(x1, *key++);
    x3 += *key++;
    x2 += *key++;
    MUL(x4, *key);
    out = (word16*)outbuf;

    *out++ = x1;
    *out++ = x3;
    *out++ = x2;
    *out = x4;

    if (log != nullptr) {
        fprintf(log, "| %04x | %04x | %04x | %04x | ---- | ---- || %04x | %04x | %04x | %04x |\n", key_p_out[0], key_p_out[1], key_p_out[2], key_p_out[3], x1, x3, x2, x4);
        fprintf(log, "========================================================================\n");
        fprintf(log, "End of encryption process.\n\n");
    }
}
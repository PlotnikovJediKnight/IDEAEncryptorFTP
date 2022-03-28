#pragma once
#include <algorithm>

#define low16(x) (x)

#define MUL(x,y) \
        ((t16 = (y)) ? \
        (x=low16(x)) ? \
         t32 = (word32)x*t16, \
         x = low16(t32), \
         t16 = t32>>16, \
         x = (x-t16)+(x<t16) \
         : \
         (x = 1-t16) \
         : \
         (x = 1-x))


#ifndef BLOCKS
    #ifndef KBYTES
        #define KBYTES 1024
    #endif

    #define BLOCKS (64*KBYTES)
#endif


#ifndef TRUE
    #define FALSE 0
    #define TRUE (!FALSE)
#endif

#ifndef min
    #define min(a,b) ( (a)<(b) ? (a) : (b) )
    #define max(a,b) ( (a)>(b) ? (a) : (b) )
#endif


#define IDEAKEYSIZE 16
#define IDEABLOCKSIZE 8
#define IDEAROUNDS 8
#define IDEAKEYLEN (6 * IDEAROUNDS + 4)

typedef unsigned char boolean; /* values are TRUE or FALSE */
typedef unsigned char byte;    /* values are 0–255 */
typedef byte* byteptr;         /* pointer to byte */
typedef char* string;          /* pointer to ASCII character string */
typedef unsigned short word16; /* values are 0–65535 */
typedef unsigned long word32;  /* values are 0–4294967295 */

typedef word16 uint16;


class IDEAEncryptor {
private:
	static IDEAEncryptor* instance_pointer;

	typedef struct {
		word16 ek[IDEAKEYLEN], dk[IDEAKEYLEN];
	}idea_ctx;

    idea_ctx c;

    uint16 mulInv(uint16);
    void ideaExpandKey(byteptr, word16[IDEAKEYLEN]);
    void ideaInvertKey(const word16[IDEAKEYLEN], word16[IDEAKEYLEN]);
    void ideaCipher(byteptr, byteptr, word16*, FILE* = nullptr);
    IDEAEncryptor();
public:
	static IDEAEncryptor* getInstance();
    void idea_key(byteptr);
    void idea_enc(byteptr, int, FILE* = nullptr);
    void idea_dec(byteptr, int, FILE* = nullptr);
};
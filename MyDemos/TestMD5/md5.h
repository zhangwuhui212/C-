#ifndef MD5_H
#define MD5_H

typedef struct {
	unsigned int state[4];                                   /* state (ABCD) */
	unsigned int count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MDString (char *string,unsigned char digest[16]);
int MD5File (char *filename,unsigned char digest[16]);
void MD5ToString(unsigned char * md5,unsigned char * hexMd5);

#endif /*MD5_H*/

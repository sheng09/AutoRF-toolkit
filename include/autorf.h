#include "sac.h"

#ifndef AUTORF
#define AUTORF

#define GOOD  1
#define BAD   0

#define MAXSTR 1024
typedef int ISRF;

typedef struct
{
	ISRF status;
	char filenm[MAXSTR];
	SACHEAD hdr;
	float  *trace;
}SACFILE;


//1.Whether Amplitude at 'O' is positive
ISRF isPPostive(SACHEAD *hdr, float *trace);

ISRF isPPostive2(SACHEAD *hdr, float *trace, float interval);

ISRF isPMax(SACHEAD *hdr, float *trace, float t1, float t2);

ISRF isSame(SACHEAD *hdr, float *trace, SACHEAD *RefHdr, float *RefTrace,
		float threshold, float pre, float suf);

#endif

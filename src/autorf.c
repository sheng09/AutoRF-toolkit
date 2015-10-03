#include <stdlib.h>
#include <math.h>
#include "sac.h"
#include "autorf.h"
#include "libnumrec.h"

//1.Whether Amplitude at 'O' is positive
ISRF isPPostive(SACHEAD *hdr, float *trace)
{
	int index ;
	index = rintf( (hdr->o - hdr->b) / (hdr->delta) );
	if( trace[index] > 0.0f )
		return GOOD;
	else
		return BAD;
}

ISRF isPPostive2(SACHEAD *hdr, float *trace, float interval)
{
	int pre, suf;
	pre = rintf( (hdr->o - hdr->b - interval ) / (hdr->delta) );
	suf = rintf( (hdr->o - hdr->b + interval ) / (hdr->delta) );
	if( integf(trace, pre, suf, hdr->delta) > 0.0f )
		return GOOD;
	else
		return BAD;
}

ISRF isPMax(SACHEAD *hdr, float *trace, float interval)
{
	float Maxt = 0.0f;
	int   MaxIndex = 0;
	fdmaxf(trace, hdr->npts , &MaxIndex);
	Maxt = MaxIndex * hdr->delta + hdr->b;
	if( Maxt >= (hdr->o - interval) && Maxt <= (hdr->o + interval) )
		return GOOD;
	else
		return BAD;
}

ISRF isSame(SACHEAD *hdr, float *trace, SACHEAD *RefHdr, float *RefTrace,
		float threshold, float pre, float suf)
{
	float value;
	int iPre, iSuf, iRefPre, iRefSuf, len, Reflen;
	iPre    = rintf( ( hdr->o    - hdr->b + pre    ) / (hdr->delta) );
	iSuf    = rintf( ( hdr->o    - hdr->b + suf    ) / (hdr->delta) );
	iRefPre = rintf( ( RefHdr->o - RefHdr->b + pre ) / (RefHdr->delta) );
	iRefSuf = rintf( ( RefHdr->o - RefHdr->b + suf ) / (RefHdr->delta) );
	len     = iSuf    - iPre    + 1;
	Reflen  = iRefSuf - iRefPre + 1;

	len     = MIN(len, Reflen);
	value = corf(trace+iPre, RefTrace + iRefPre, len);
	if( value >= threshold)
		return GOOD;
	else
		return BAD;
}


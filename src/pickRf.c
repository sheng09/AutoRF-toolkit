#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "autorf.h"
#include "sac.h"
#include "liberrmsg.h"

static char HMSG[]="\
Description: pick GOOD RFs given SACfiles\n\
\n\
Usage: %s SACfiles [-P[d|a<interval>]] [-M<interval>] -G<good list> -D<bad list>\n\
\n\
-P    rf must be positive around 'O'.\n\
        -Pd amplitude must be positive at 'O' point\n\
        -Pa<interval> amplitude integral within [O-interval, O+interval] must be positive\n\
\n\
-M<t1/t2>\n\
      rf must have its max amplitude within [O+t1, O+t2].\n\
-G    output file for good files list\n\
-D    output file for bad files list\n\
";



int main(int argc, char *argv[])
{
	FILE *fpgood = NULL,  *fpbad = NULL;
	char *strgood = NULL, *strbad = NULL;
	char fgP = 0, fgMax = 0, fggood = 0, fgbad = 0;
	SACFILE		data;
	int i;
	int ntotal =0, npicked =0, nbad = 0;
	float intervalP = 1.0f, t1 = 0.0f, t2 = 0.5f;
	for( i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'G':
					strgood = &( argv[i][2] );
					fggood  = 1;
					break;
				case 'D':
					strbad  = &( argv[i][2] );
					fgbad   = 1;
					break;
				case 'P':
					fgP = argv[i][2];
					if(fgP == 'a')
						sscanf(argv[i],"-Pa%f", &intervalP);
					break;
				case 'M':
					fgMax = 1;
					sscanf(argv[i],"-M%f/%f", &t1, &t2);
					break;
				case 'H':
					fprintf(stderr, HMSG, argv[0]);
					exit(1);
					break;
			}
		}
	}

	if( fggood != 1 || fgbad != 1 || (fgP == 0 && fgMax == 0) )
	{
		perrmsg("", ERR_MORE_ARGS);
		fprintf(stderr, HMSG, argv[0]);
		exit(1);
	}

	if( NULL == ( fpgood =  fopen(strgood,"w") ))
	{
		perrmsg(strgood, ERR_OPEN_FILE);
		exit(1);
	}
	if( NULL == ( fpbad =  fopen(strbad,"w") ))
	{
		perrmsg(strbad, ERR_OPEN_FILE);
		exit(1);
	}

	for( i = 1; i < argc; ++i)
	{
		if(argv[i][0] != '-')
		{
			++ntotal;
			data.status = GOOD;
			data.trace  = read_sac(argv[i], &(data.hdr) );

			if(data.status == GOOD && fgP != 0)
			{
				if(fgP == 'd')
					data.status = isPPostive( &(data.hdr), data.trace);
				else if(fgP == 'a')
					data.status = isPPostive2( &(data.hdr), data.trace, intervalP);
			}
			if(data.status == GOOD && fgMax == 1)
			{
				data.status = isPMax( &(data.hdr), data.trace, t1, t2);
			}

			if(data.status == GOOD)
			{
				++npicked;
				fprintf(fpgood,"%s\n", argv[i]);
			}
			else
			{
				++nbad;
				fprintf(fpbad, "%s\n", argv[i] );
			}
		}
	}
	fprintf(stderr, "Total: %d Picked: %d Bad: %d\n", ntotal, npicked, nbad);
	free(data.trace);
	fclose(fpgood);
	fclose(fpbad);
	return 0;
}
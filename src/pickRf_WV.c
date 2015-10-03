#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "autorf.h"
#include "sac.h"
#include "liberrmsg.h"
#include "libnumrec.h"

static char HMSG[]="\
Description: Given number of sac files, pick good one by comparing it with their stacked RF.\n\
\n\
Usage: %s -L<filelist> -G<good list> -D<bad list> -T<threshold> [-R<reference rf>] \n\
\n\
-L     file that contain sac filenames line by line\n\
-G     output file for good sacfiles\n\
-D     output file for bad sac files\n\
-T     threshold for correlation between each sacfile and their stacked RF. (0.6)\n\
[-R]   optional  output file for stacked RF.\n\
";



int main(int argc, char *argv[])
{
	FILE  *fplst = NULL, *fpgood = NULL,  *fpbad = NULL;
	char *strRef = NULL, *strgood = NULL, *strbad = NULL;
	int  fgRef = 0, fglst =0, fggood = 0, fgbad = 0, fgthreshold = 0;

	int i;
	char  *strlst = NULL;
	int  nlst;
	char line[MAXSTR];
	SACFILE *saclst = NULL;

	float **trlst;
	int len;
	float *refRF = NULL;
	float value, threshold = 0.6;
	int   ngood = 0, nbad = 0;
	for( i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'R':
					strRef = &( argv[i][2] );
					fgRef  = 1;
					break;
				case 'G':
					strgood = &( argv[i][2] );
					fggood  = 1;
					break;
				case 'D':
					strbad  = &( argv[i][2] );
					fgbad   = 1;
					break;
				case 'L':
					strlst = &(argv[i][2]);
					fglst  = 1;
					break;
				case 'T':
					sscanf(argv[i],"-T%f", &threshold);
					fgthreshold = 1;
					break;
				case 'H':
					fprintf(stderr, HMSG,  argv[0]);
					exit(0);
					break;
			}
		}
	}
	if(fglst != 1  || fggood != 1 || fgbad != 1)
	{
		perrmsg("",ERR_MORE_ARGS);
        	fprintf(stderr, HMSG,argv[0]);
        	exit(1);
	}
	if( fgthreshold == 1 && (threshold < 0.0f ||  threshold > 1.0f ) )
	{
		perrmsg("-T<threshold>",ERR_BAD_ARGS);
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
	// Read file that contains sac filenames
	if( NULL == ( fplst =  fopen(strlst,"r") ))
	{
		fprintf(stderr, "%s\n", strlst);
		perrmsg( strlst, ERR_READ_FILE);
        	exit(1);
	}

	// Allocate memory for sac files
	for(nlst = 0;;)
	{
		if( NULL == fgets(line, MAXSTR, fplst) )
			break;
		++nlst;
	}
	fseek(fplst, 0, SEEK_SET);
	saclst = (SACFILE *) calloc(nlst, sizeof(SACFILE) );
	trlst  = (float **)  calloc(nlst, sizeof(float *));

	// Read in all the sac files
	for(i = 0; i < nlst; ++i)
	{
		saclst[i].status = GOOD;
		fscanf(fplst, "%s\n", (saclst[i].filenm) );
		saclst[i].trace = read_sac( saclst[i].filenm, &(saclst[i].hdr) );
		//printf(" %d %s\n", i, saclst[i].filenm);
		trlst[i] = saclst[i].trace;
	}
	len = saclst[0].hdr.npts;
	refRF = (float *) calloc(len, sizeof(float));
	// Stack to get the reference RF
	nrootStack(trlst, nlst, len, &refRF, 2.0f);

	//Output the stacked reference RF
	if(fgRef == 1)
		write_sac(strRef, saclst[0].hdr, refRF);

	// Compare rfs with reference RF
	for(i = 0; i < nlst; ++i)
	{
		value = corf(saclst[i].trace, refRF, len);
		if(value >= threshold)
		{
			++ngood;
			fprintf(fpgood, "%s %f\n", saclst[i].filenm, value);
		}
		else
		{
			++nbad;
			fprintf(fpbad, "%s %f\n", saclst[i].filenm, value);
		}
	}

	for(i = 0; i < nlst; ++i)
	{
		free(saclst[i].trace);
	}
	fprintf(stderr, "Total: %d Picked: %d Bad %d\n", nlst, ngood, nbad );
	fclose(fplst);
	fclose(fpgood);
	fclose(fpbad);
	return 0;
}
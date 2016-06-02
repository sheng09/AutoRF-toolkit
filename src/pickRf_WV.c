#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "autorf.h"
#include "sac.h"
#include "liberrmsg.h"
#include "libnumrec.h"

static char HMSG[]="\
Description: Given number of sac files, pick good one by comparing it with reference RF.\n\
             [-r] option to input a reference RF.\n\
             [-R] option to use stacked waveform of all input RF as reference RF.\n\
\n\
Usage: %s -L<filelist> -G<good list> -D<bad list> -T<threshold> [-R<reference rf>]\n\
         [-r<input reference rf>] [-C<tmark/t1/t2>] \n\
\n\
-L     file that contain sac filenames line by line\n\
-G     output file for good sacfiles\n\
-D     output file for bad sac files\n\
-T     threshold for correlation between each sacfile and their stacked RF. (0.6)\n\
\n\
[-C<tmark/t1/t2>]  time interval for compare.\n\
       tmark=o\n\
[-r]   inout a reference RF.\n\
[-R]   optional  output file for stacked RF.\n\
";



int main(int argc, char *argv[])
{
	FILE  *fplst = NULL, *fpgood = NULL,  *fpbad = NULL;
	char *strRef = NULL, *str_Ref_I = NULL, *strgood = NULL, *strbad = NULL;
	int  fgRef = 0, fgRef_I = 0, fglst =0, fggood = 0, fgbad = 0, fgthreshold = 0;

	int i;
	char  *strlst = NULL;
	int  nlst;
	char line[MAXSTR];
	SACFILE *saclst = NULL;
	SACFILE sacref;

	float **trlst;
	int len;
	float *refRF = NULL;
	float value, threshold = 0.6;
	int   ngood = 0, nbad = 0;

	char  cutFlag = 0;
	float cutB=0.0f, cutE=0.0f;
	int   cutBd = 0, cutEd = 0;
	for( i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'C':
					sscanf(argv[i],"-C%c/%f/%f", &cutFlag, &cutB, &cutE);
					break;
				case 'R':
					strRef = &( argv[i][2] );
					fgRef  = 1;
					break;
				case 'r':
					str_Ref_I = &( argv[i][2] );
					fgRef_I = 1;
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
		if( 0 == fscanf(fplst, "%s\n", (saclst[i].filenm) ) )
		{
			perrmsg( strlst, ERR_READ_FILE);
        	exit(1);
		}
		saclst[i].trace = read_sac( saclst[i].filenm, &(saclst[i].hdr) );

		//Add by wangsheng 2015/11/08
		//for(j = 0; j < (saclst[i]).hdr.npts; ++j)
		//{
		//	//printf("%f \n", (saclst[i]).hdr.depmax);
		//	//printf("%f\n", ((saclst[i]).trace)[(int) rintf( (saclst[i].hdr.o - saclst[i].hdr.b)/(saclst[i].hdr.delta) ) ]);
		//	(saclst[i].trace)[j] = (saclst[i].trace)[j] / 
		//		((saclst[i]).trace)[(int) rintf( (saclst[i].hdr.o - saclst[i].hdr.b)/(saclst[i].hdr.delta) ) ];
		//}
		//printf(" %d %s\n", i, saclst[i].filenm);
		trlst[i] = saclst[i].trace;
	}
	len = saclst[0].hdr.npts;


	if( fgRef_I == 0 )
	{
		refRF = (float *) calloc(len, sizeof(float));
		// Stack to get the reference RF
		nrootStack(trlst, nlst, len, &refRF, 1.0f);
		//Output the stacked reference RF
		if(fgRef == 1)
			write_sac(strRef, saclst[0].hdr, refRF);
	}
	else
	{
		refRF = read_sac( str_Ref_I, &(sacref.hdr) );
	}

	// Compare rfs with reference RF
	if(cutFlag == 'o' )
	{
		cutBd = (int) rintf( (cutB + (saclst[0].hdr.o - saclst[0].hdr.b) )/saclst[0].hdr.delta );
		cutEd = (int) rintf( (cutE + (saclst[0].hdr.o - saclst[0].hdr.b) )/saclst[0].hdr.delta );
		if(cutBd < 0)
			cutBd = 0;
		else if( cutBd > len-1 )
			cutBd = len-1;

		if(cutEd < 0)
			cutEd = 0;
		else if( cutEd > len-1 )
			cutEd = len-1;
	}
	else
	{
		cutBd = 0;
		cutEd = len;
	}
	for(i = 0; i < nlst; ++i)
	{
		//value = corf(saclst[i].trace, refRF, len);
		value = corf_2(saclst[i].trace, refRF, len, cutBd, cutEd );
		//printf("%d %d %f\n\n", cutBd, cutEd, value );
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
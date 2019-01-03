//- #include <libctiny.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(linker, "/subSystem:Console")
//#pragma comment(linker, "/noDefaultLib")
//#pragma comment(lib, "minicrt")


int FClose(FILE * fp1, FILE * fp2, int ret) {
	if (fp1) fclose(fp1);
	if (fp2) fclose(fp2);
	return ret;
}

int FBClose(FILE * fp1, FILE * fp2, int *Buf1, int *Buf2, int ret) {
	FClose(fp1, fp2, ret);
	free(Buf1); free(Buf2);
	return ret;
}

int bytediff(int b1, int b2) {
	int i;
	unsigned char *p1 = (char*)&b1, *p2 = (char*)&b2;
	for (i = 0; i < 4; i++)
		if (*p1++ != *p2++) break;
	return i;
}

//#define __CMB_64BIT__ 1
#ifdef __CMB_64BIT__
	#define INTEG __int64
	#define FSEEK _fseeki64
	#define FTELL _ftelli64
#else
	//faster 2GB version
	#define INTEG int
	#define FSEEK fseek
	#define FTELL ftell
#endif

INTEG seeknRead(FILE* fp1, FILE *fp2, INTEG offset) {
	unsigned int b1, b2, got1, got2;
	int k, origin = SEEK_SET;
	INTEG roundsize = offset;

	if (offset < 0) {
		origin = SEEK_END;
		roundsize = -offset;
		//offset = ((roundsize - 1) & 3) +1;  // it's the the end of dword.
		offset = -(~offset & 3) -1; // not x == -(x + 1) == -x -1
	}
	else if (offset) offset = (offset -1) >> 2 << 2;

	if (FSEEK(fp1, offset, origin)) return FClose(fp1, fp2, -7);
	if (FSEEK(fp2, offset, origin)) return FClose(fp1, fp2, -8);

	b1 = 0; got1 = fread(&b1, sizeof(b1), 1, fp1);
	b2 = 0; got2 = fread(&b2, sizeof(b2), 1, fp2);
	//DEBUG printf("b1: %0.8x, b2: %.08x, offset: %d\n", b1, b2, offset);

	if (b1 != b2) {
		if(roundsize) roundsize = (roundsize - 1) >> 2 << 2; // just in case
		return FClose(fp1, fp2, 1 + roundsize + bytediff(b1, b2));
	}
	return 0;
}

INTEG scanRead(FILE* fp1, FILE *fp2, INTEG sz1) {
	int b1, b2;
	INTEG ret, sz2;
	sz2 = 0; b1 = 0; b2 = 0;
	if (ret = seeknRead(fp1, fp2, 0)) return ret;

	while(!ret && (sz2 += 4) < (sz1 - 4)) {
		fread(&b1, sizeof(b1), 1, fp1);
		fread(&b2, sizeof(b2), 1, fp2);
		if (b1 != b2)
			ret = FClose(fp1, fp2, 1 + sz2 + bytediff(b1, b2));
	}

	return ret;
}


//+MSVS extension for sequential read
#define FPREADMODE "rbS"
#define DIVBLOCK 7
#define REC9_64 0x1c71c71c71c71c72LL
#define REC9_32 0x1c71c71cLL
#define REC7_64 0x2492492492492492LL
#define REC7_32 0x24924925LL

#define SMALLFRY 0x1000
#define SMALLFOX 0x10000
#define BUFFSIZE 0x100000


int main(int argn, char *args[])
//int mainCRTStartup(int argn, char *args[])
{
	FILE *fp1, *fp2;
	int *Buf1, *Buf2;
	int b1, b2, got1, got2;

	char *p, *s = args[0];

	INTEG sz1, sz2, ret;
	INTEG seek1, seek2;
	INTEG k;

	int i;

	if (argn < 3) {
		//p = strchr(s, '\\'); // while((*p) && *p != '\\') p++;
		//
		//while(p) {
		//	s = ++p;
		//	p = strchr(p, '\\'); // while ((*p) && *p != '\\') p++;
		//}
		p = s;
		while(*p && (*p != '\\')) p++;
		while(*p) {
			s = ++p;
			while (*p && (*p != '\\')) p++;
		}
		//return 0;
		printf("\n");
		printf("  Copyright(C) 2003-2010, PT Softindo Jakarta\n");
		printf("  email: aa _at_ softindo.net\n");
		printf("  All rights reserved.\n");
		printf("\n");
		printf("  Binary comparison between 2 files (max. size: 2GB)\n\n");
		printf("  Usage: %s file1 file2\n\n", s);
		printf("  Return value is SILENTLY put in errorlevel (NO OUPUT PRINTED)\n");
		//printf("  \n");
		printf("\t0 = file1 and file2 are identical\n");
		printf("\tpositive = position where difference found (1-based)\n");
		printf("\tnegative = any other differences or errors\n\n");
		printf("Press any key to continue..\n");
		//getch();
		return -argn -19;
	}

	if ((fp1 = fopen(args[1], FPREADMODE)) == NULL) return -1;
	if ((fp2 = fopen(args[2], FPREADMODE)) == NULL) return FClose(fp1, NULL, -2);

	if (FSEEK(fp1, 0LL, SEEK_END)) return FClose(fp1, NULL, -4);
	if (FSEEK(fp2, 0LL, SEEK_END)) return FClose(fp1, NULL, -5);

	sz1 = FTELL(fp1); sz2 = FTELL(fp2);
	if (sz1 != sz2) return FClose(fp1, fp2, -6);

	if (!sz1) return FClose(fp1, fp2, sz1);

	if (ret = seeknRead(fp1, fp2, -sz1)) return ret;
	if (ret = seeknRead(fp1, fp2, sz1 >> 1)) return ret;

	if (sz1 > SMALLFRY) {
		k = (sz1-1) / DIVBLOCK;
		sz2 = k * (DIVBLOCK + 1);
		while ((sz2 -= k) >= k)
			if (ret = seeknRead(fp1, fp2, sz2))
				return ret;
	}

	if (sz1 > SMALLFOX) {
		Buf1 = (int *)malloc(BUFFSIZE);//if (!Buf1) return FClose(fp1, fp2, -21);
		Buf2 = (int *)malloc(BUFFSIZE);//if (!Buf2) return FClose(fp1, fp2, -22);

		if (Buf1 && Buf2) {
			if (FSEEK(fp1, 0LL, SEEK_SET)) return FClose(fp1, fp2, -7);
			if (FSEEK(fp2, 0LL, SEEK_SET)) return FClose(fp1, fp2, -8);
			sz2 = 0;
			while(!ret && (sz2 < (sz1 - 4))) {
				got1 = fread(Buf1, 1, BUFFSIZE, fp1);
				got2 = fread(Buf2, 1, BUFFSIZE, fp2);

				// got1 & got2 must be: positive > 0, identical, and mult4
				if (got1 != got2) return FBClose(fp1, fp2, Buf1, Buf2, -13);
				if (!got1) return FBClose(fp1, fp2, Buf1, Buf2, -14);
				//if (got1 & 3) return FBClose(fp1, fp2, Buf1, Buf2, -15);


				for (i = 0; i < got1 >> 2; i++)
					if (Buf1[i] != Buf2[i])
						return FBClose(fp1, fp2, Buf1, Buf2,
							1 + sz2 + i*4 + bytediff(Buf1[i], Buf2[i]));
				sz2 += i*4;
			}
			free(Buf1);	free(Buf2);
		}
		else
			ret = scanRead(fp1, fp2, sz1);
	}
	else
		ret = scanRead(fp1, fp2, sz1);

	return ret;

}

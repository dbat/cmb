// changed name from compare.c to cmbx.c
//- #include <libctiny.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
//#pragma comment(linker, "/noDefaultLib")
//#pragma comment(linker, "/subSystem:Console")
//- #pragma comment(lib, "minicrt.lib")

#define posdiff_sse posdiff_sse
#define posdiff_mmx posdiff_mmx
#define uns unsigned
extern unsigned int __stdcall f1CompareMem3(uns * S1, uns * S2, int count);
extern unsigned int __stdcall posdiff_sse(uns * S1, uns * S2, int count);
extern unsigned int __stdcall posdiff_mmx(uns * S1, uns * S2, int count);

int _printerrCodes() {
	char * ERRORS[] = {
	//return (-e + 1) >> 1;
		"identical",           //0 0
		"open",                //1 1,2
		"get-info",            //2 3,4
		"target is a directory", //3 5,6
		"size is different",   //4 7,8
		"memory allocation"	,  //5 9,10
		"seek to beginning",   //6 11,12
		"random seek",         //7 13,14
		"read fetch",          //8 15,16
		"reading to buffer",   //9 17,18
		"read buffer count",   //10 19,20
		"argument"             //11 21,22
	};
	int i, k;
	printf("\n");
	printf("  Odd number means File1 error, next even number means File2 error\n");
	printf("  For example:\n");
	printf("\t-1\tFile1 %s error (invalid, not found, etc.)\n", ERRORS[1]);
	printf("\t-2\tFile2 %s error\n", ERRORS[1]);
	printf("\n");
	printf("  Return codes:\n");
	printf("\tCode\tDescription\n");
	printf("\t0\tfiles are %s\n", ERRORS[0]);
	//-OK for (i = 1; i <= 11; i++)
	//-OK 	for (k = 1; k <= 2; k++)
	//-OK 		printf("\t-%d \tFile%d %s error\n", i*2-2+k, k, ERRORS[i]);
	for (i = 1; i <= 11; i++)
		for (k = 1; k <= 2; k++)
			if (k & 1)
				if (i == 4)
					printf("\t-%d \t%s\n", i*2-2+k, ERRORS[i]);
				else
					printf("\t-%d \t%s error\n", i*2-2+k, ERRORS[i]);
	
	printf("\tAny positive value: position of difference found (1-based)\n");
	printf("\n");
	printf("  Note:\tPosition of difference is 1-based since 0 means identical and\n");
	printf("\twould be indistinguishable with difference at pos-0 in 0-based\n");
	return 0;
}

char * _getFilename(char * args0) {
	char *p, *s = args0;
	p = s;
	while(*p && (*p != '\\')) p++;
	while(*p) {
		s = ++p;
		while (*p && (*p != '\\')) p++;
	}
	return s;
}

int _showHelp(char * args0, int ret) {
	//char * bar = "============================================";
	char * fn = _getFilename(args0);
	printf("\n");
	//printf("  Copyright(C) 2003-2010, PT Softindo Jakarta\n");
	//printf("  email: aa _at_ softindo.net\n");
	//printf("  All rights reserved.\n"); //printf("\n");
	printf("  Version: 1.2.5 (windows console)\n");
	printf("  Created: 2001.01.01, Last Revised: 2011.03.05\n");
	//printf("  Revised: 2001.02.01\n");
	printf("\n");
	printf("  Synopsys:\n\tBinary comparison between 2 files\n\t(See important notes below for filesize > 2GB)\n\n");
	printf("  Usage:\n");
	printf("\t%s -e\n", fn);
	printf("\t%s File1 File2\n", fn);
	printf("\n");
	printf("\t(Returns 0 if File1 and File2 content are identical)\n");
	printf("\n");
	printf("  Notes:\n");
	printf("\tUsing SSE2 instruction, needs Pentium4+ CPU\n");
	printf("\tFor lesser CPU (MMX) you have to recompile by yourself\n");
	printf("\n");
	printf("\t64-bit return value is _silently_ put in ERRORLEVEL\n\t(NO OUPUT WILL BE PRINTED/DISPLAYED)\n");
	printf("\n");
	printf("\tReturn code type:\n");
	printf("\t    0 = file1 and file2 are identical\n");
	printf("\t    positive = position where difference found (1-based)\n");
	printf("\t    negative = any other differences or errors\n");
	printf("\n");
	printf("\ttype \"%s -e\" to see error return code list\n");
	printf("\n");
	printf("  Important:\n");
	printf("    If filesize is > 2GB, return value might get misrepresented.\n");
	printf("    Since ERRORLEVEL is an integer, pos. of difference truncated\n");
	printf("    at low-DWORD, thus it might be 0 or interpreted as negative.\n");
	printf("    In that case (only) this program will print out the position.\n");
	printf("\n");
	//printf("Press any key to continue..\n");
	//printf("  %s\n",bar);
	//_printerrCodes();
	//printf("\n");
	printf("\n");
	printf("  Copyright(C) 2003-2009, PT SOFTINDO, Jakarta\n");
	printf("  email: aa _at_ softindo.net\n");
	printf("  All rights reserved.\n");	//printf("\n");
	//getchar();
	printf("\n");
	return ret;
}

#define uns unsigned
#define __PRL__ printf("line %d\n", __LINE__);
#define __PRET__ __PRL__; return __LINE__;


__int64 CloseF(HANDLE f1, HANDLE f2, __int64 ret) {
	if (f1) CloseHandle(f1);
	if (f2) CloseHandle(f2);
	//__PRL__
	//if (ret < 0)
	//	printf("File error: %I64d\n", ret);
	return ret;
}

__int64 CloseB(uns * Buf1, uns * Buf2, __int64 ret) {
	if (Buf1) free (Buf1);
	if (Buf2) free (Buf2);
	//__PRL__
	//if (ret < 0)
	//	printf("Buffer error: %I64d\n", ret);
	return ret;
}

__int64 CloseFB(HANDLE f1, HANDLE f2, uns * Buf1, uns * Buf2, __int64 ret) {
	CloseB(Buf1, Buf2, 0);
	CloseF(f1, f2, 0);
	//if (ret < 0)
	//	printf("Buffer error: %I64d\n", ret);
	return ret;
}

#define Err_NOARGUMENT1 -21
#define Err_NOARGUMENT2 -22
#define __FILE_IS_EQUAL 0
#define Err_CREATEFILE1 -1
#define Err_CREATEFILE2 -2
#define Err_GETFILEINFO1 -3
#define Err_GETFILEINFO2 -4
#define Err_CHECKISDIR1 -5
#define Err_CHECKISDIR2 -6
#define Err_SIZEDIFFERENT -7
#define Err_FILETOOBIG -8
#define Err_MALLOCBUF1 -9
#define Err_MALLOCBUF2 -10
#define Err_FSEEKBEGIN1 -11
#define Err_FSEEKBEGIN2 -12
#define Err_FILESEEK1 -13
#define Err_FILESEEK2 -14
#define Err_READFETCH1 -15
#define Err_READFETCH2 -16
#define Err_READBUFF1 -17
#define Err_READBUFF2 -18
#define Err_READBCOUNT -19
#define Err_READBZERO -20


#define DO2_CLOSEALL(f) { CloseHandle(f##1); CloseHandle(f##2); }
#define DO2_CLOSEALLBUF(b) { if(b##1) free(b##1); if(b##2) free(b##2); }

#define DO__CLOSERR(f, n, e) CloseF(f##1, f##2, Err_##e##n);
#define DO__CLOSERRFB(f, b, n, e) CloseFB(f##1, f##2, b##1, b##2, Err_##e##n);

#define DO__CREATEFILE(f, n) { \
	f##n = CreateFile(args[n], GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, \
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); \
	if (f##n == INVALID_HANDLE_VALUE) return Err_CREATEFILE##n; \
	}

#define DO__GETFILEINFO(f, i, n) \
	if (!GetFileInformationByHandle(f##n, &i##n)) return DO__CLOSERR(f, n, GETFILEINFO)

#define DO__CHECKISDIR(f, i, n) \
	if (i##n.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return DO__CLOSERR(f, n, CHECKISDIR)

#define DO__MALLOCF(b, F, n) \
	if (!(b##n = (int *)malloc(BUFFSIZE))) return DO__CLOSERRFB(F, b, n, MALLOCBUF)

#define DO__MALLOC(b, n) \
	if (!(b##n = (int *)malloc(BUFFSIZE))) DO2_CLOSEALLBUF(b)

#define DO__FILESEEK(Ex, f, ofs, n) \
	if (!SetFilePointer##Ex(f##n, ofs, NULL, origin) == INVALID_SET_FILE_POINTER) return DO__CLOSERR(f, n, FILESEEK)

#define DO__FSEEKBEGIN(f, n) \
	if (SetFilePointer(f##n, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return DO__CLOSERR(f, n, FSEEKBEGIN)

#define DO__READFILE(f, B, sz, c, n, e) \
	if(!ReadFile(f##n, B##n, sz, (LPDWORD)c##n, NULL)) return DO__CLOSERR(f, n, e)

#define DO__READFETCH(f, b, c, n) \
	DO__READFILE(f, b, sizeof(b##n), c, n, READFETCH)

#define DO__READBUFF(f, B, c, n) \
	DO__READFILE(f, B, BUFFSIZE, c, n, READBUFF)

#define RETURN_pret \
	{ if ((__int64)ret > 0x7fffffffLL) printf("%I64d", ret); return ret; }


#define DO2M_1a(M, a) { DO__##M(a, 1) DO__##M(a, 2) }
#define DO2M_2a(M, a, b) { DO__##M(a, b, 1) DO__##M(a, b, 2) }
#define DO2M_3a(M, a,b,c) { DO__##M(a,b,c,1) DO__##M(a,b,c,2) }
#define DO2M_4a(M, a,b,c,d) { DO__##M(a,b,c,d,1) DO__##M(a,b,c,d,2) }
#define DO2M_5a(M, a,b,c,d,e) { DO__##M(a,b,c,d,e,1) DO__##M(a,b,c,d,e,2) }

#define _EQU1(i, a) ((i##1.a) == (i##2.a))
#define _EQU2(i, a, b) ((i##1.a == i##2.a) && (i##1.b == i##2.b))
#define _EQU3(i, a, b, c) (((i##1).a == (i##2).a) && ((i##1).b == (i##2).b) && ((i##1).c == (i##2).c))
#define _NEQU(i, a) ((i##1.a) != (i##2.a))
//#define _NEQU2(i, a, b) !_EQU2(i, a, b)
#define _NEQU2(i, a, b) (((i##1).a != (i##2).a) || ((i##1).a != (i##2).a))


#define DIVBLOCK 7
#define SMALLFRY 0x1000 	// 4KB
#define SMALLFILE 0x10000	// 64KB
//#define BUFFSIZE 0x100000	// 1024KB
#define BUFFSIZE 0x1000000	// 16MB

_printargs(int argn, char * args[]) {
	int i;
	printf("\n");
	printf("arguments: %d\n. argn");
	for (i = 0; i < argn; i++)
		printf("args[%d]: %s\n", i, args[i]);
	printf("\n");
}

int bytediff(int b1, int b2) {
	int i;
	unsigned char *p1 = (char*)&b1, *p2 = (char*)&b2;
	for (i = 0; i < 4; i++)
		if (*p1++ != *p2++) break;
	return i;
}

_pretty(__int64 ret) {
	if ((__int64)ret > 0x7fffffffLL)
		printf("%I64d", ret);
	return ret;
}

DWORD GetFilePointer (HANDLE hFile) {
	return SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
}

__int64 GetFilePointerEx (HANDLE hFile) {
    LARGE_INTEGER L1 = {0}, L2 = {0};
    SetFilePointerEx(hFile, L1, &L2, FILE_CURRENT);
    return L2.QuadPart;
}

__int64 seeknReadTest(HANDLE f1, HANDLE f2, __int64 offset) {
	unsigned int b1, b2, got1, got2;
	__int64 roundsize = offset;
	LARGE_INTEGER L;
	int origin = FILE_BEGIN;
	//printf("offset: %.016I64x (%I64d)\n", offset, offset);
	//printf("rnsize: %.016I64x (%I64d)\n", roundsize, roundsize);

	if (offset > 0x7fffffff) {
		offset = (offset -1) >> 2 << 2;
		L.QuadPart = offset;
		//OK: DO2M_3a(FILESEEK, SetFilePointerEx, f, L)
		DO2M_3a(FILESEEK, Ex, f, L)
	}
	else {
		if (offset < 0) {
			origin = FILE_END;
			roundsize = -offset;
			//offset = ((roundsize - 1) & 3) +1;  // it's the the end of dword.
			offset = -(~offset & 3) -1; // not x == -(x + 1) == -x -1
		}
		DO2M_3a(FILESEEK,, f, offset)
	}

	b1=0; b2=0;
	DO2M_3a(READFETCH, f, &b, &got)
	//printf("b1: %0.8x, b2: %.08x, offset: %d\n", b1, b2, offset);

	if (b1 != b2) {
		if(roundsize) roundsize = (roundsize - 1) >> 2 << 2; // just in case
		//printf("b1: %0.8x, b2: %.08x, offset: %I64d\n", b1, b2, 1 + roundsize + bytediff(b1, b2));
		return CloseF(f1, f2, 1 + roundsize + bytediff(b1, b2));
	}
	return 0;
}

__int64 scanRead(HANDLE f1, HANDLE f2, __int64 sz1) {
	__int64 ret, sz2;
	int b1, b2, got1, got2;
	sz2 = 0; b1 = 0; b2 = 0;

	if (ret = seeknReadTest(f1, f2, 0)) return ret;

	while(!ret && (sz2 += 4) < (sz1 - 4)) {
		DO2M_3a(READFETCH, f, &b, &got)
		if (b1 != b2)
			ret = CloseF(f1, f2, 1 + sz2 + bytediff(b1, b2));
	}
	return ret;
}

int main(int argn, char * args[]) {
	HANDLE f1, f2;
	__int64 sz1, sz2;
	__int64 k, ret = 19690919; //30802030; //15101540;
	BY_HANDLE_FILE_INFORMATION info1, info2;
	unsigned *Buf1 = NULL, *Buf2 = NULL;
	int i, b1, b2, got1, got2;

	int ms;
	LARGE_INTEGER L;
	//:speedtest:// LARGE_INTEGER tic, tac, Hz;

	if (args[1] && strcmp(args[1],"-e") == 0) return _printerrCodes();
	if (argn < 3) return _showHelp(args[0], -argn -20);

	//:speedtest:// QueryPerformanceFrequency(&Hz);
	//:speedtest:// QueryPerformanceCounter(&tic);

	DO2M_1a(CREATEFILE, f) //DO__CREATEFILE_ALL(f)
	DO2M_2a(GETFILEINFO, f, info) //DO__GETFILEINFO_ALL(f, info)
	DO2M_2a(CHECKISDIR, f, info) //DO__CHECKISDIR_ALL(f, info)

	if _EQU3(info, dwVolumeSerialNumber, nFileIndexLow,	nFileIndexHigh)
		return CloseF(f1, f2, __FILE_IS_EQUAL);

	if _NEQU2(info, nFileSizeLow, nFileSizeHigh)
		return CloseF(f1, f2, Err_SIZEDIFFERENT);

	//if (info1.nFileSizeHigh || (int)info1.nFileSizeLow < 0)
	//	return CloseF(f1, f2, Err_FILETOOBIG);
	//
	//if (info1.nFileSizeLow == 0)
	//	return CloseF(f1, f2, __FILE_IS_EQUAL);


	//sz1 = (__int64)(info1.nFileSizeHigh << 31) | info1.nFileSizeLow;
	L.LowPart = info1.nFileSizeLow;
	L.HighPart = info1.nFileSizeHigh;
	sz1 = L.QuadPart;
	if (!sz1) return CloseF(f1, f2, __FILE_IS_EQUAL);

	//printf("sz1: %.016I64x (%I64d)\n", sz1, sz1);
    //sz1 = -sz1;
    //sz1 = -sz1;
	//printf("sz1: %.016I64x (%I64d)\n", sz1, sz1);

	if (ret = seeknReadTest(f1, f2, -sz1)) return ret;
	if (ret = seeknReadTest(f1, f2, sz1 >> 1)) return ret;

	if (sz1 > SMALLFRY) {
		k = (sz1 - 1) / DIVBLOCK;
		sz2 = k * (DIVBLOCK + 1);
		while ((sz2 -= k) >= k)
			if (ret = seeknReadTest(f1, f2, sz2))
				return ret;
	}
	if (sz1 > SMALLFILE) {
		DO2M_1a(MALLOC, Buf)
		if (Buf1 && Buf2) {
			DO2M_1a(FSEEKBEGIN, f)
			sz2 = 0;
			while(!ret && (sz2 < (sz1 - 4))) {
				//printf("sz1: %I64d, sz2: %I64d\n", sz1, sz2);
				DO2M_3a(READBUFF, f, Buf, &got)
				// got1 & got2 must be: positive > 0, identical, and mult4
				if (got1 != got2) return CloseFB(f1, f2, Buf1, Buf2, Err_READBCOUNT);
				if (!got1) return CloseFB(f1, f2, Buf1, Buf2, Err_READBZERO);
				//if (got1 & 3) return CloseFB(f1, f2, Buf1, Buf2, Err_READMISMATCH);

				//OK - with debug message
				//for (i = 0; i < got1 >> 2; i++) {
				//	b1 = Buf1[i]; b2 = Buf2[i];
				//	if (i % 0x10000 == 0)
				//		printf("b1: %0.8x, b2: %.08x, offset: %d\n", b1, b2, sz2 + i*4);
				//	if (Buf1[i] != Buf2[i]) {
				//		printf("DIFF - b1: %0.8X, b2: %.08X, offset: %.08X (%d) \n", b1, b2, sz2 + i*4, sz2 + i*4);
				//		return FBClose(fp1, fp2, Buf1, Buf2,
				//			1 + sz2 + i*4 + bytediff(Buf1[i], Buf2[i]));
				//		break;
				//	}
				/*
				for (i = 0; i < got1 >> 2; i++) {
					//if (i % 0x10000000 == 0) {
					//	b1 = Buf1[i]; b2 = Buf2[i];
					//	//printf("scan b1: %0.8x, b2: %.08x, dw-offset: %I64x (%I64d)\n", b1, b2, sz2 + i*4, sz2 + i*4);
					//}
					if (Buf1[i] != Buf2[i]) {
						//return CloseFB(f1, f2, Buf1, Buf2,
						//	1 + sz2 + i*4 + bytediff(Buf1[i], Buf2[i]));
						//__PRL__
						ret = sz2 + i*4 + bytediff(Buf1[i], Buf2[i]); // 0-based
						ret++; // 1-based, since 0 means identical and would be indistinguished with differnce in 0-pos
						//b1 = Buf1[i]; b2 = Buf2[i];
						//printf("DIFF - b1: %0.8X, b2: %.08X, offset: %I64X (%I64d) \n", b1, b2, ret, ret);
						break;
						//printf("b1: %0.8x, b2: %.08x, offset: %d\n", b1, b2, sz2 + i*4);
						//printf("b1: %0.8x, b2: %.08x, offset: %I64d\n", Buf1[i], Buf2[i], ret);
					}
				}
				sz2 += i*4;
				*/
				i = posdiff_sse(Buf1, Buf2, got1);
				if (!i) sz2 += got1;
				else {
				  ret = sz2 + i;
				  break;
				}
			}
		}
		else ret = scanRead(f1, f2, sz1);
		
		DO2_CLOSEALLBUF(Buf)
	}
	else ret = scanRead(f1, f2, sz1);
	CloseF(f1, f2, 0);
	//:speedtest:// QueryPerformanceCounter(&tac);
	//:speedtest:// sz1 = (tac.QuadPart - tic.QuadPart) * 1000000 / Hz.QuadPart;
	//:speedtest:// ms = sz1;
	//:speedtest:// printf("timer: %d.%d seconds\n", ms/1000000, ms%1000000);
	if (ret > 0x7fffffff) printf("%I64d", ret);
	return ret;
}



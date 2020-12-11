#ifndef _ALG_RADIX_H
#define _ALG_RADIX_H

#include "simple.h"

void all_countsort_smp(long q,
			int *lKey,
			int *lSorted,
			int R,
			int bitOff, int m,
			THREADED);

#define all_radixsort_smp(a,b,c,d)   all_radixsort_smp_s3(a,b,c,d)
void all_radixsort_smp_s3(long q,
			   int *lKeys,
			   int *lSorted,
			   THREADED);
void all_radixsort_smp_s2(long q,
			   int *lKeys,
			   int *lSorted,
			   THREADED);
void all_radixsort20_smp_s1(long q,
			     int *lKeys,
			     int *lSorted,
			     THREADED);
void all_radixsort20_smp_s2(long q,
			     int *lKeys,
			     int *lSorted,
			     THREADED);

void all_radixsort_check(long q,
			 int *lSorted);

#endif


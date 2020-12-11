#include <stdio.h>

#include "alg_radix_smp.h"

#define bits(x,k,j) ((x>>k) & ~(~0<<j))

/****************************************************/
void all_countsort_smp(long q,
			int *lKey,
			int *lSorted,
			int R,
			int bitOff, int m,
			THREADED)
/****************************************************/
/* R (range)      must be a multiple of SMPS */
/* q (elems/proc) must be a multiple of SMPS */
{
    register int
	j,
	k,
        last, temp,
	offset;
    
    int *myHisto,
        *mhp,
        *mps,
        *psHisto,
        *allHisto;

    long x;

    myHisto  = (int *)node_malloc(THREADS*R*sizeof(int), TH);
    psHisto  = (int *)node_malloc(THREADS*R*sizeof(int), TH);

    mhp = myHisto + MYTHREAD*R;

    for (k=0 ; k<R ; k++)
      mhp[k] = 0;
    
    pardo(x, 0, q, 1)
      mhp[bits(lKey[x],bitOff,m)]++;

    node_Barrier();

    pardo(k, 0, R, 1) {
      last = psHisto[k] = myHisto[k];
      for (j=1 ; j<THREADS ; j++) {
	temp = psHisto[j*R + k] = last + myHisto[j*R +  k];
	last = temp;
      }
    }

    allHisto = psHisto+(THREADS-1)*R;

    node_Barrier();
    
    offset = 0;

    mps = psHisto + (MYTHREAD*R);
    for (k=0 ; k<R ; k++) {
      mhp[k]  = (mps[k] - mhp[k]) + offset;
      offset += allHisto[k];
    }

    node_Barrier();
    
    pardo(x, 0, q, 1) {
      j = bits(lKey[x],bitOff,m);
      lSorted[mhp[j]] = lKey[x];
      mhp[j]++;
    }

    node_Barrier();

    node_free(psHisto, TH);
    node_free(myHisto, TH);
}


/****************************************************/
void all_countsort_smp_aux(int q,
			    int *lKey,
			    int *lSorted, int* auxKey, int* auxSorted,
			    int R,
			    int bitOff, int m,
			    THREADED)
/****************************************************/
/* R (range)      must be a multiple of SMPS */
/* q (elems/proc) must be a multiple of SMPS */
{
    register int
	j,
	k,
        last, temp,
	offset;
    
    int *myHisto,
        *mhp,
        *mps,
        *psHisto,
        *allHisto;

    myHisto  = (int *)node_malloc_l(THREADS*R*sizeof(int), TH);
    psHisto  = (int *)node_malloc_l(THREADS*R*sizeof(int), TH);

    mhp = myHisto + MYTHREAD*R;

    for (k=0 ; k<R ; k++)
      mhp[k] = 0;
    
    pardo(k, 0, q, 1)
      mhp[bits(lKey[k],bitOff,m)]++;

    node_Barrier();

    pardo(k, 0, R, 1) {
      last = psHisto[k] = myHisto[k];
      for (j=1 ; j<THREADS ; j++) {
	temp = psHisto[j*R + k] = last + myHisto[j*R +  k];
	last = temp;
      }
    }

    allHisto = psHisto+(THREADS-1)*R;

    node_Barrier();
    
    offset = 0;

    mps = psHisto + (MYTHREAD*R);
    for (k=0 ; k<R ; k++) {
      mhp[k]  = (mps[k] - mhp[k]) + offset;
      offset += allHisto[k];
    }

    node_Barrier();
    
    pardo(k, 0, q, 1) {
      j = bits(lKey[k],bitOff,m);
      lSorted[mhp[j]] = lKey[k];
      auxSorted[mhp[j]] = auxKey[k];
      mhp[j]++;
    }

    node_Barrier();

    node_free(psHisto, TH);
    node_free(myHisto, TH);
}


/****************************************************/
void all_radixsort_check(long q,
			 int *lSorted)
/****************************************************/
{
  long i;

  for (i=1; i<q ; i++) 
    if (lSorted[i-1] > lSorted[i]) {
      fprintf(stderr,
	      "ERROR: q:%d lSorted[%6d] > lSorted[%6d] (%6d,%6d)\n",
	      q,i-1,i,lSorted[i-1],lSorted[i]);
    }
}

/****************************************************/
void all_radixsort_smp_s3(long q,
			   int *lKeys,
			   int *lSorted,
			   THREADED)
/****************************************************/
{
  int *lTemp;

    lTemp = (int *)node_malloc_l(q*sizeof(int), TH);
			
    all_countsort_smp(q, lKeys,   lSorted, (1<<11),  0, 11, TH);
    all_countsort_smp(q, lSorted, lTemp,   (1<<11), 11, 11, TH);
    all_countsort_smp(q, lTemp,   lSorted, (1<<10), 22, 10, TH);

    node_free(lTemp, TH);
}

/****************************************************/
void all_radixsort_smp_s2(long q,
			   int *lKeys,
			   int *lSorted,
			   THREADED)
/****************************************************/
{
  int *lTemp;

    lTemp = (int *)node_malloc_l(q*sizeof(int), TH);
			
    all_countsort_smp(q, lKeys,   lTemp,   (1<<16),  0, 16, TH);
    all_countsort_smp(q, lTemp,   lSorted, (1<<16), 16, 16, TH);

    node_free(lTemp, TH);
}

/****************************************************/
void all_radixsort20_smp_s1(long q,
			     int *lKeys,
			     int *lSorted,
			     THREADED)
/****************************************************/
{
    all_countsort_smp(q, lKeys,   lSorted, (1<<20),  0, 20, TH);
}

/****************************************************/
void all_radixsort20_smp_s2(long q,
			     int *lKeys,
			     int *lSorted,
			     THREADED)
/****************************************************/
{
  int *lTemp;

    lTemp = (int *)node_malloc_l(q*sizeof(int), TH);
			
    all_countsort_smp(q, lKeys,   lTemp,   (1<<10),  0, 10, TH);
    all_countsort_smp(q, lTemp,   lSorted, (1<<10), 10, 10, TH);

    node_free(lTemp, TH);
}

/****************************************************/
void all_radixsort_smp_aux_s3(int q,
			       int *lKeys,
			       int *lSorted, int* auxKey, int* auxSorted,
			       THREADED)
/****************************************************/
{
    int *lTemp, *lTemp2;

    lTemp = (int *) node_malloc_l(q*sizeof(int), TH);
    lTemp2 = (int *) node_malloc_l(q*sizeof(int), TH);
		
    all_countsort_smp_aux(q, lKeys, lSorted, auxKey, auxSorted, (1<<11),  0, 11, TH);
    all_countsort_smp_aux(q, lSorted, lTemp, auxSorted, lTemp2, (1<<11), 11, 11, TH);
    all_countsort_smp_aux(q, lTemp, lSorted, lTemp2, auxSorted, (1<<10), 22, 10, TH);

    node_free(lTemp, TH);
    node_free(lTemp2, TH);
}










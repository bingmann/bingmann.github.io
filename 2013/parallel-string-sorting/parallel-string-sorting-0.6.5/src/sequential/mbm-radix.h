/*
   Hybrid American flag sort (with stack control), a radix sort
   algorithm for arrays of character strings by McIlroy, Bostic,
   and McIlroy.

   P. M. McIlroy, K. Bostic, and M. D. McIlroy. Engineering radix
   sort. Computing Systems, 6(1):5-27, 1993.

   The code presented in this file has been tested with care but is
   not guaranteed for any purpose. The writer does not offer any
   warranties nor does he accept any liabilities with respect to
   the code.

   Stefan Nilsson, 2 jan 1997.

   Laboratory of Information Processing Science
   Helsinki University of Technology
   Stefan.Nilsson@hut.fi

   Updated to use std::swap by Timo Bingmann in 2013
*/

namespace mbm_radixsort {

typedef unsigned char* string;

enum { SIZE = 1024, THRESHOLD = 10 };

typedef struct { string *sa; int sn, si; } mbmstack_t;

static void simplesort(string a[], int n, int b)
{
   int i, j;
   string tmp;

   for (i = 1; i < n; i++)
     for (j = i; j > 0 && stringtools::scmp(a[j-1]+b, a[j]+b) > 0; j--)
         { tmp = a[j]; a[j] = a[j-1]; a[j-1] = tmp; }
}

static void rsorta(string *a, int n, int b)
{
#define push(a, n, i)   sp->sa = a, sp->sn = n, (sp++)->si = i
#define pop(a, n, i)    a = (--sp)->sa, n = sp->sn, i = sp->si
#define stackempty()    (sp <= stack)
        mbmstack_t      stack[SIZE], *sp = stack, *oldsp, *bigsp;
        string          *pile[256], *ak, *an, r;
        static int      count[256], cmin, nc;
        int             *cp, c, cmax;

        push(a, n, b);

        while(!stackempty()) {
                pop(a, n, b);
                if(n < THRESHOLD) {
                        simplesort(a, n, b);
                        continue;
                }
                an = a + n;
                if(nc == 0) {                       /* untallied? */
                        cmin = 255;                 /* tally */
                        for(ak = a; ak < an; ) {
                                c = (*ak++)[b];
                                if(++count[c] == 1 && c > 0) {
                                        if(c < cmin) cmin = c;
                                        nc++;
                                }
                        }
                        if(sp+nc > stack+SIZE) {     /* stack overflow */
                                  rsorta(a, n, b);
                                  continue;
                        }
                }
                oldsp = bigsp = sp, c = 2;         /* logartihmic stack */
                pile[0] = ak = a+count[cmax=0];    /* find places */
                for(cp = count+cmin; nc > 0; cp++, nc--) {
                         while(*cp == 0) cp++;
                         if (*cp > 1) {
                                  if(*cp > c) c = *cp, bigsp = sp;
                                  push(ak, *cp, b+1);
                         }
                         pile[cmax = cp-count] = ak += *cp;
                }
                std::swap(*oldsp, *bigsp);
                an -= count[cmax];                 /* permute home */
                count[cmax] = 0;
                for(ak = a; ak < an; ak += count[c], count[c] = 0) {
                        r = *ak;
                        while(--pile[c = r[b]] > ak)
                            std::swap(*pile[c], r);
                        *ak = r;
                                /* here nc = count[...] = 0 */
                }
        }
}

void mbm_radix(string a[], size_t n)
{ rsorta(a, n, 0); }

#undef push
#undef pop
#undef stackempty

CONTESTANT_REGISTER(mbm_radix,
                    "mbm/radixsort",
                    "MSD Radix Sort by P. M. McIlroy, K. Bostic, and M. D. McIlroy")

} // namespace mbm_radixsort

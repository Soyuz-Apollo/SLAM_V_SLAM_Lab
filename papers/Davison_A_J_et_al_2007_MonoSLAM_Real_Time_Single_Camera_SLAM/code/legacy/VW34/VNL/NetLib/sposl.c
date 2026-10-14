#include "f2c.h"
#include "netlib.h"

/* Table of constant values */
static integer c__1 = 1;

/* Subroutine */ void sposl_(a, lda, n, b)
const real *a;
const integer *lda, *n;
real *b;
{
    /* Local variables */
    static integer k;
    static real t;

/*     sposl solves the single precision symmetric positive definite    */
/*     system a * x = b                                                 */
/*     using the factors computed by dpoco or dpofa.                    */
/*                                                                      */
/*     on entry                                                         */
/*                                                                      */
/*        a       single precision(lda, n)                              */
/*                the output from spoco or spofa.                       */
/*                                                                      */
/*        lda     integer                                               */
/*                the leading dimension of the array  a .               */
/*                                                                      */
/*        n       integer                                               */
/*                the order of the matrix  a .                          */
/*                                                                      */
/*        b       single precision(n)                                   */
/*                the right hand side vector.                           */
/*                                                                      */
/*     on return                                                        */
/*                                                                      */
/*        b       the solution vector  x .                              */
/*                                                                      */
/*     error condition                                                  */
/*                                                                      */
/*        a division by zero will occur if the input factor contains    */
/*        a zero on the diagonal.  technically this indicates           */
/*        singularity but it is usually caused by improper subroutine   */
/*        arguments.  it will not occur if the subroutines are called   */
/*        correctly and  info .eq. 0 .                                  */
/*                                                                      */
/*     to compute  inverse(a) * c  where  c  is a matrix                */
/*     with  p  columns                                                 */
/*           call spoco(a,lda,n,rcond,z,info)                           */
/*           if (rcond is too small .or. info .ne. 0) go to ...         */
/*           do 10 j = 1, p                                             */
/*              call sposl(a,lda,n,c(1,j))                              */
/*        10 continue                                                   */
/*                                                                      */
/*     linpack.  this version dated 08/14/78 .                          */
/*     cleve moler, university of new mexico, argonne national lab.     */

/*     subroutines and functions */
/*     blas saxpy,sdot */

/*     solve trans(r)*y = b */

    for (k = 0; k < *n; ++k) {
        t = sdot_(&k, &a[k * *lda], &c__1, b, &c__1);
        b[k] = (b[k] - t) / a[k + k * *lda];
    }

/*     solve r*x = y */

    for (k = *n - 1; k >= 0; --k) {
        b[k] /= a[k + k * *lda];
        t = -b[k];
        saxpy_(&k, &t, &a[k * *lda], &c__1, b, &c__1);
    }
} /* sposl_ */

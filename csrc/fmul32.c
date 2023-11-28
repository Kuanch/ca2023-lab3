#include <stdio.h>
#include <stdint.h>


int32_t mmul(register int32_t a, register int32_t b) {
    register int32_t r = 0;
    a = a << 1; /* to counter last right shift */
    do {
        if((b & 1) != 0) {
            r = r + a;
        }
        b = b >> 1;
        r = r >> 1;
    } while(b != 0);
    return r;
}


/* float32 multiply */
int32_t fmul32(int32_t ia, int32_t ib) {
    /* define sign */
    int32_t sr = (ia ^ ib) >> 31;
    /* define mantissa */
    int32_t ma = (ia & 0x7fffff) | 0x800000;
    int32_t mb = (ib & 0x7fffff) | 0x800000;
    int32_t mr;
    /* define exponent */
    int32_t ea = ((ia >> 23) & 0xff);
    int32_t eb = ((ib >> 23) & 0xff);
    int32_t er;
    /* special values */
    if(ea == 0xff) {
        if(ma == 0x800000 && eb != 0) {
            return 0x7f800000 | sr << 31;
        }
        return 0x7f800001;
    }
    if(eb == 0xff) {
        if(mb == 0x800000 && ea != 0) {
            return 0x7f800000 | sr << 31;
        }
        return 0x7f800001;
    }
    if(ea == 0) {
        return sr << 31;
    }
    if(eb == 0) {
        return sr << 31;
    }
    /* multiplication */
    register int32_t mrtmp = mmul(ma, mb);
    register int32_t ertmp = ea + eb - 127;
    /* realign mantissa */
    register int32_t mshift = (mrtmp >> 24) & 1;
    mr = mrtmp >> mshift;
    er = ertmp + mshift;
    /* overflow and underflow */
    if(er <= 0) {
        return sr << 31;
    }
    if(er >= 0xff) {
        return 0x7f800000 | sr << 31;
    }
    /* result */
    return (sr << 31) | ((er & 0xff) << 23) | (mr & 0x7fffff);
}


int main() {
    float a = 1.65;
    float b = 2.5;
    int32_t ia = *(int32_t *) &a;
    int32_t ib = *(int32_t *) &b;
    *((volatile int *) (4)) = fmul32(ia, ib);

    a = 0;
    b = 2.5;
    ia = *(int32_t *) &a;
    ib = *(int32_t *) &b;
    *((volatile int *) (8)) = fmul32(ia, ib);

    a = 1.33;
    b = 4.2556;
    ia = *(int32_t *) &a;
    ib = *(int32_t *) &b;
    *((volatile int *) (12)) = fmul32(ia, ib);

    return 0;
}
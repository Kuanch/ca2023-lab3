#include <stdio.h>

#include <stdint.h>


uint32_t highestbit(register uint32_t x) {
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    /* count ones (population count) */
    x -= ((x >> 1) & 0x55555555);
    register int32_t num3333 = 0x33333333;
    x = ((x >> 2) & num3333) + (x & num3333);
    x = ((x >> 4) + x) & 0x0f0f0f0f;
    x += (x >> 8);
    x += (x >> 16);
    return x & 0x7f;
}


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
    int32_t ia = *(int32_t *) &a;    // 0 01111111 10000000000000000000000
    int32_t ib = *(int32_t *) &b;    // 0 10000000 01000000000000000000000
    int32_t mul_r = fmul32(ia, ib);
}
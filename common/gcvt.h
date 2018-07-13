#include <stdint.h>

#ifdef STANDALONE_TEST
#include <stdio.h>
#include <math.h>
#endif

struct Gcvt
{
    union TValue
    {
        double n;
        uint64_t u64;
        struct {
            uint32_t lo;
            uint32_t hi;
        } u32;
    };
    
    uint32_t nd[128];
    int32_t ndlo;
    int32_t ndhi;

    int32_t nd_div2k(uint32_t* nd, int32_t ndlo, int32_t ndhi,
                     uint32_t k) 
    {
        while (k >= 9) {
            uint32_t i = ndhi & 127, carry = 0;
            for (;;) {
                uint32_t val = nd[i];
                nd[i] = (val >> 9) + carry;
                carry = (val & 0x1ff) * 1953125;
                if (i == (ndlo & 127)) break;
                i = (i - 1) & 127;
            }
            if (carry) nd[--ndlo & 127] = carry;
            k -= 9;
        }
        if (k) {
            uint32_t mask = (1U << k) - 1, mul = 1000000000 >> k;
            uint32_t i = ndhi & 127, carry = 0;
            for (;;) {
                uint32_t val = nd[i];
                nd[i] = (val >> k) + carry;
                carry = (val & mask) * mul;
                if (i == (ndlo & 127)) break;
                i = (i - 1) & 127;
            }
            if (carry) nd[--ndlo & 127] = carry;
        }
        return ndlo;
    }

    int32_t nd_mul2k(uint32_t* nd, int32_t ndhi, uint32_t k,
                     uint32_t carry_in) 
    {
        while (k >= 29) {
            for (uint32_t i = 0; i <= (uint32_t)ndhi; i++) {
                uint64_t val = ((uint64_t)nd[i] << 29) | carry_in;
                carry_in = (uint32_t)(val / 1000000000);
                nd[i] = (uint32_t)val - carry_in * 1000000000;
            }
            if (carry_in) {
                nd[++ndhi] = carry_in; carry_in = 0;
            }
            k -= 29;
        }
        if (k) {
            for (uint32_t i = 0; i <= (uint32_t)ndhi; i++) {
                uint64_t val = ((uint64_t)nd[i] << k) | carry_in;
                carry_in = (uint32_t)(val / 1000000000);
                nd[i] = (uint32_t)val - carry_in * 1000000000;
            }
            if (carry_in) nd[++ndhi] = carry_in;
        }
        return ndhi;
    }

    static void divmod9(char* p, uint32_t n) 
    {
        for (uint32_t i = 9; i--; n /= 10) {
            p[i] = '0' + (n % 10);
        }
    }

    void nd_print(char* p, uint32_t* nd, int32_t ndlo, int32_t ndhi) 
    {
        int32_t i;
        for (i = ndhi; i >= 0; --i) {
            divmod9(p, nd[i & 127]); p += 9;
        }
        *p++ = '.';
        for (; i >= ndlo; --i) {
            divmod9(p, nd[i & 127]); p += 9;
        }
        *p = 0;
    }

    void _convert(char* buf, double n)
    {
        TValue t;
        t.n = n;
        if ((t.u32.hi << 1) >= 0xffe00000)
        {
            if (((t.u32.hi & 0x000fffff) | t.u32.lo) != 0)
            {
                strcat(buf, "NaN");
            } else
            {
                strcat(buf, "Infinity");
            }
        }
        else
        {
            uint32_t nd[128];
            int32_t ndlo = 0;
            int32_t ndhi = 0;
            int32_t e = (t.u32.hi >> 20) & 0x7ff;
            nd[0] = t.u32.hi & 0xfffff;
            if (e == 0) {
                e++;
            } else {
                nd[0] |= 0x100000;
            }
            e -= 1043;
            if (t.u32.lo) {
                e -= 32;
                nd[0] = (nd[0] << 3) | (t.u32.lo >> 29);
                ndhi = nd_mul2k(nd, ndhi, 29, t.u32.lo & 0x1fffffff);
            }
            if (e >= 0) {
                ndhi = nd_mul2k(nd, ndhi, (uint32_t)e, 0);
            } else {
                ndlo = nd_div2k(nd, ndlo, ndhi, (uint32_t)-e);
            }
            nd_print(buf, nd, ndlo, ndhi);
        }
    }

    static void _convertInt(char* buf, int v)
    {
        // d >= 0
        char tbuf[16];
        char* p = tbuf;
        char* q;

        if (v == 0)
        {
            buf[0] = '0';
            buf[1] = 0;
            return;
        }
    
        while (v > 0)
        {
            int q = v / 10;
            int r = v - q*10;
            *p++ = '0' + r;
            v = q;
        }

        q = buf;
        do
        {
            *q++ = *--p;
        } while (p != tbuf);
        *q = 0;
    }

    char* gcvt(double v, int ndigit, char* buf)
    {
        char tbuf[1154];
        char* s;
        char* e;
        char* q;
        int sci = 0;
    
        q = buf;
        *q = 0;

        if (v < 0)
        {
            *q++ = '-';
            *q = 0;
            _convert(tbuf, -v);
        }
        else
        {
            _convert(tbuf, v);
        }
        
        //printf("raw %s\n", tbuf);
    
        s = tbuf;
        while (*s == '0') ++s;
        e = s;
        if (*e == '.')
        {
            // v < 1.0
            if (*++e != '0')
            {
                *q++ = '0';
                *q++ = '.';
                while (*e && ndigit)
                {
                    --ndigit;
                    *q++ = *e++;
                }
                *q = 0;

                // clean up any trailing zero
                while (q[-1] == '0') *--q = 0;
            }
            else
            {
                // scentific
                int ex = 1;
                while (*e && *e == '0') { ++e; ++ex; }
                if (*e)
                {
                    *q++ = *e++;
                    *q++ = '.';
                    while (*e && --ndigit) *q++ = *e++;
                    *q++ = 'e';
                    *q++ = '-';
                    _convertInt(q, ex);
                }
                else
                {
                    *q++ = '0';
                    *q++ = '.';
                    *q = 0;
                }
            }
        }
        else
        {
            int nd = 0;
            while (*e && *e != '.')
            {
                if (nd == ndigit)
                {
                    // scientific
                    sci = 1;
                    break;
                }
                ++nd;
                *q++ = *e++;
            }
            *q++ = '.';
            if (*e == '.')
            {
                ++e;
                while (nd < ndigit)
                {
                    *q++ = *e++;
                    ++nd;
                }

                // clean up any trailing zero
                while (q[-1] == '0') *--q = 0;
            }
            *q = 0;

            if (sci)
            {
                int ex = ndigit-1;
                q = buf;
                if (*q == '-') ++q;
                e = s;
                *q++ = *e++;
                *q++ = '.';
                while (--ndigit) *q++ = *e++;
                *q++ = 'e';
                *q++ = '+';
                while (*e && *e != '.') { ++e; ++ex; }
                _convertInt(q, ex);
            }
        }

        //printf("gcvt(%.16g) = %s\n", v, buf);

        return buf;
    }
};

#ifdef STANDALONE_TEST
int main(int argc, char** argv)
{
    double v = 1.0;
    char buf[128];
    char buf2[128];
    
    if (argc > 1)
    {
        v = atof(argv[1]);
    }

    printf("printf v = %g\n", v);

    _gcvt(v, 16, buf);
    printf("gcvt %s\n", buf);

    u_gcvt(v, 16, buf2);
    printf("u_gcvt %s\n", buf2);
        
    return 0;
}
#endif

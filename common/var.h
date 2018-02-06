/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (©) Voidware 2016-2017.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 * 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS," WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 * 
 *  contact@voidware.com
 */

#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include <string.h> //strcmp
#include <math.h> // floor
#include <stdlib.h>
#include "cutils.h"
#include "types.h"

struct var
{
    enum type
    {
        var_null = 0,
        var_string,
        var_double,
        var_int,
        var_count
    };

    type _type;
    
    var() : _type(var_null) {}
    var(const char* s) : _type(var_string) { _enstring(s); }
    var(const char* s, size_t sz) : _type(var_string) { _enstring(s, sz); }
    var(const std::string& s) : _type(var_string) { _enstring(s.c_str()); }
    var(double d) : _type(var_double), _d(d) {}
    var(int i) : _type(var_int), _i(i) {}
    var(int64 i) : _type(var_int), _i(i) {}
    var(bool v) 
    {
        // masquerade int as bool
        _type = v ? var_int : var_null;
        _i = (int)v;
    }
    
    var(const var& v) { _dub(v); }
    ~var() { purge(); }

    var& operator=(const var& v) 
    {
        purge();
        _dub(v);
        return *this;
    }

    void _dub(const var& v)
    {
        _type = v._type;
        switch (_type)
        {
        case var_string:
            {
                // move over the string content; donate pattern
                var* vp = (var*)&v; // XX const cast
                _s = vp->_donate();
            }
            break;
        case var_double:
            _d = v._d;
            break;
        case var_int:
            _i = v._i;
            break;
        }
    }

    var copy() const
    {
        var v;
        v._type = _type;
        switch (_type)
        {
        case var_string:
            v._enstring(_s);
            break;
        case var_double:
            v._d = _d;
            break;
        case var_int:
            v._i = _i;
            break;
        }
        return v;
    }

    bool operator==(const var& v) const
    {
        bool res = _type == v._type;
        if (res) 
        {
            switch(_type)
            {
            case var_null: // match
                break;
            case var_string:
                res = !strcmp(_s, v._s);
                break;
            case var_double:
                res = _d == v._d;
                break;
            case var_int:
                res = _i == v._i;
                break;
            }
        }
        return res;
    }

    bool operator==(const std::string& s) const
    {
        return _type == var_string && s == _s;
    }

    bool operator!=(const var& v) const { return !(*this == v); }

#if 0
    bool operator<(const var& v) const
    {
        bool res = _type == v._type;
        if (res) 
        {
            res = false; 
            switch(_type)
            {
            case var_null: 
                // null !< null
                break;
            case var_string:
                res = strcmp(_s, v._s) < 0;
                break;
            case var_double:
                res = _d < v._d;
                break;
            case var_int:
                res = _i < v._i;
                break;
            }
        }
        else
        {
            // order by type, when different
            res = _type < v._type;
        }
        return res;
    }
#endif


    bool isVoid() const { return _type == var_null; }
    bool isString() const { return _type == var_string; }
    
    // true if not void
    explicit operator bool() const { return !isVoid(); }

    int64 toInt(bool round = false) const
    {
        // convert to int, if possible
        int64 val = 0;
        switch (_type)
        {
        case var_string:
            if (_s) val = strtoll(_s, 0, 10);
            break;
        case var_double:
            // NB: doesn't round -X.5 properly
            val = round ? (int64)floor(_d + 0.5) : (int64)_d;
            break;
        case var_int:
            val = _i;
            break;
        }
        return val;
    }

    double toDouble() const
    {
        // convert to double, if possible
        double val = 0;
        switch (_type)
        {
        case var_string:
            if (_s) val = atof(_s);
            break;
        case var_double:
            val = _d;
            break;
        case var_int:
            val = (double)_i;
            break;
        }
        return val;
    }
    
    std::string toString(bool quoteStrings = false) const
    {
        switch (_type)
        {
        case var_string:
            if (quoteStrings)
            {
                std::string s;
                s = '"';
                s += _s;
                s += '"';
                return s;
            }
            else return _s;
        case var_double:
            {
                char buf[32];
                sprintf(buf, "%f", _d);
                return buf;
            }
            break;
        case var_int:
            {
                char buf[32];
		sprintf(buf, "%" _PRId64, _i);
                return buf;
            }
            break;
        }
        return "null";
    }

    var toNumber() const
    {
        if (_type == var_string)
        {
            var v;
            const char* s = _s;
            
            // if not numeric string, v is void
            v.parse(&s);
            return v;
        }
        else return *this;
    }

    var neg() const
    {
        var v = toNumber();
        switch (v._type)
        {
        case var_int:
            v._i = -v._i;
            break;
        case var_double:
            v._d = -v._d;
            break;
        }
        return v;
    }

    bool isZero() const
    {
        // if numerically zero (not including void or "0")
        bool res = false;
        switch (_type)
        {
        case var_double:
            res = _d == 0;
            break;
        case var_int:
            res = _i == 0;
            break;
        }   
        return res;
    }

    bool isTrue() const
    {
        // void value is false, otherwise true unless value == 0
        return !isVoid() && !isZero();
    }

    bool parse(const char** s)
    {
        purge();
        
        const char* sp = *s;
        bool res = scanReal(&sp);
        if (res)
        {
            _type = var_double;
            _d = atof(*s);
            *s = sp;
        }
        else if ((res = scanInt(&sp)) != false)
        {
            _type = var_int;
            _i = strtoll(*s, 0, 10);
            *s = sp;
        }
        return res;
    }

    void purge()
    {
        if (isString())
        { 
            delete [] _s;        
            _s = 0;
        }
        _type = var_null;
    }

    void _enstring(const char* s)
    {
        return _enstring(s, strlen(s));
    }

    void _enstring(const char* s, size_t sz)
    {
        _s = new char[sz + 1];
        memcpy(_s, s, sz);
        _s[sz] = 0;
    }

    static bool scanReal(const char **s)
    {
        /* allowable forms:
         * [-]?[0-9]*\.[0-9]*(e[-]?[0-9]+)?
         * [-]?[0-9]+e-[0-9]+
         * 
         */

        const char* p = *s;
        int n;
        int point = 0;

        if (*p == '-') ++p;
        n = 0;
        while (u_isdigit(*p)) { ++n; ++p; }
        if (!n && *p != '.') 
        {
            // no digits and no .
            return false;
        }
        if (*p == '.') 
        {
            ++p;
            point = 1;
            while (u_isdigit(*p)) { ++n; ++p; }

            // insist on at least one digit after point.
            if (!n) return false; 
        }
    
        if (*p == 'E' || *p == 'e') // exp
        {
            if (!n) return false;
            ++p;
            if (*p == '+' || *p == '-') ++p;
            n = 0;
            while (u_isdigit(*p)) { ++n; ++p; }
            if (!n) return false;
        }
        else if (!point) return false;

        *s = p;
        return true;
    }

    static bool scanInt(const char** s)
    {
        /* only scan integers. allow forms:
         * -?[0-9]+
         * allow 0[Xb][0-9A-F]+
         */

        const char* p = *s;
        int n;
        
        if (*p == '-') ++p;
        n = 0;

        while (u_isdigit(*p)) { ++p; ++n; }
    
        if (!n) return false;
        *s = p;
        return true;
    }

    friend var operator+(const var& a, const var& b)
    {
        var a1 = a.toNumber();
        var b1 = b.toNumber();
        switch (a1._type)
        {
        case var_null:
            return b1;
        case var_double:
            if (b1._type == var_double) a1._d += b1._d;
            else if (b1._type == var_int) a1._d += (double)b1._i;
            break;
        case var_int:
            if (b1._type == var_double) a1._i += (int)b1._d;
            else if (b1._type == var_int) a1._i += b1._i;
            break;
        }
        return a1;
    }

    friend var operator-(const var& a, const var& b)
    {
        var a1 = a.toNumber();
        var b1 = b.toNumber();
        switch (a1._type)
        {
        case var_null:
            return b1.neg();
        case var_double:
            if (b1._type == var_double) a1._d -= b1._d;
            else if (b1._type == var_int) a1._d -= (double)b1._i;
            break;
        case var_int:
            if (b1._type == var_double) a1._i -= (int)b1._d;
            else if (b1._type == var_int) a1._i -= b1._i;
            break;
        }
        return a1;
    }

    friend var operator*(const var& a, const var& b)
    {
        var a1 = a.toNumber();
        var b1 = b.toNumber();

        if (!a1 || !b1) return var(); // null

        switch (a1._type)
        {
        case var_double:
            if (b1._type == var_double) a1._d *= b1._d;
            else if (b1._type == var_int) a1._d *= (double)b1._i;
            break;
        case var_int:
            if (b1._type == var_double) a1._i *= (int)b1._d;
            else if (b1._type == var_int) a1._i *= b1._i;
            break;
        }
        return a1;
    }

    friend var operator/(const var& a, const var& b)
    {
        var a1 = a.toNumber();
        var b1 = b.toNumber();

        // null/x = null and x/null = null and x/0 = null
        if (!a1 || !b1 || b1.isZero()) return var();

        switch (a1._type)
        {
        case var_double:
            if (b1._type == var_double) a1._d /= b1._d;
            else if (b1._type == var_int) a1._d /= (double)b1._i;
            break;
        case var_int:
            {
                double v(a1._i);
                if (b1._type == var_double) v /= b1._d;
                else if (b1._type == var_int) v /= b1._i;

                int vi = (int)v;
                if (vi == v)
                {
                    // can remain as int
                    a1._i = vi;
                }
                else
                {
                    // promote to double
                    a1._d = v;
                    a1._type = var_double;
                }
            }
            break;
        }
        return a1;
    }

    friend bool operator>(const var& a, const var& b)
    { return a.toDouble() > b.toDouble(); }

    friend bool operator>=(const var& a, const var& b)
    { return a.toDouble() >= b.toDouble(); }

    friend bool operator<(const var& a, const var& b)
    { return a.toDouble() < b.toDouble(); }

    friend bool operator<=(const var& a, const var& b)
    { return a.toDouble() <= b.toDouble(); }

    union 
    {
        char*           _s;
        double          _d;
        int64           _i;
    };

    friend std::ostream& operator<<(std::ostream& os, const var& v)
    { return os << v.toString(); }

private:

    char* _donate()
    {
        assert(_type == var_string);
        char* s = _s;
        _s = 0;
        _type = var_null;
        return s;
    }


};




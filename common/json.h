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

#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

#include <string>
#include "logged.h"
#include "rsstream.h"
#include "lock.h"
#include "types.h"

template<typename MT> struct JSONB: public rapidjson::Document
{
    bool        _changed;
    mutable MT  _lock;

    typedef rapidjson::Document parentT;
    typedef Locker<MT> jLock;
    typedef rapidjson::Value Value;

    JSONB()
    {
        _changed = false;
    }

    bool isDefined(const char* tag) const { return HasMember(tag); }
    bool remove(const char* tag) { return RemoveMember(tag); }

    struct ValueBase
    {
        JSONB*          _a = 0; // allocating document, if defined
        JSONB*          _json;
        const char*     _tag;

        AllocatorType&  allocator()
        {
            return _a ? _a->GetAllocator() : _json->GetAllocator();
        }
    };
    
    struct StringValue: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;

        StringValue(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;
        }

        StringValue(JSONB& host, const char* tag, JSONB& a)
        {
            this->_a = &a;
            _json = &host;
            _tag = tag;
        }

        operator const char*() const { return toString(); }
        void operator=(const char* s) { fromString(s); }
        
        bool stringIf(const char** sp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = mi != _json->MemberEnd();
            if (res)
                *sp = mi->value.GetString();
            return res;
        }

        const char* toString() const
        {
            const char* res = "";
            stringIf(&res);
            return res;
        }

        void fromString(const char* s)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd())
                mi->value.SetString(s, this->allocator());
            else
            {
                Value v;
                v.SetString(s, this->allocator());

                // XX making a constant ref doesn't seem to work...
                Value k;
                //k.SetString(rapidjson::StringRef(_tag));
                k.SetString(_tag, this->allocator());
                
                _json->AddMember(k, v, this->allocator());
            }
            _json->_changed = true;
        }
    };

    struct IntValue: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;

        IntValue(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;
        }

        void operator=(int v) { fromInt(v); }
        operator int() const { return toInt(); }

        bool intIf(int* vp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = (mi != _json->MemberEnd());
            if (res)
                *vp = mi->value.GetInt();
            return res;
        }

        int toInt() const
        {
            int v = 0;
            intIf(&v);
            return v;
        }

        void fromInt(int v)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd()) mi->value.SetInt(v);
            else
                _json->AddMember(StringRefType(_tag), v, this->allocator());
            _json->_changed = true;
        }
    };

    static bool toBool(const Value& v)
    {
        bool r = false;
        
        // string "true" counts as bool true
        if (v.IsString())
        {
            r = strcmp(v.GetString(), "true") == 0;
        }
        else if (v.IsNumber())
        {
            double dv = v.GetDouble();
            if (dv) r = true;
        }
        else if (v.IsBool())
        {
            r = v.GetBool();
        }
        return r;
    }

    struct BoolValue: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;

        BoolValue(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;
        }

        void operator=(bool v) { fromBool(v); }
        operator bool() const { return toBool(); }

        bool boolIf(bool* vp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = (mi != _json->MemberEnd());
            if (res)
                *vp = JSONB::toBool(mi->value);
            
            return res;
        }

        bool toBool() const
        {
            bool v = false;
            boolIf(&v);
            return v;
        }

        void fromBool(bool v)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd()) mi->value.SetInt(v);
            else
                _json->AddMember(StringRefType(_tag), v, this->allocator());
            _json->_changed = true;
        }
    };

    struct Int64Value: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;

        Int64Value(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;
        }

        void operator=(int64 v) { fromInt(v); }
        operator int64() const { return toInt(); }

        bool intIf(int64* vp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = (mi != _json->MemberEnd());
            if (res)
                *vp = mi->value.GetInt64();
            return res;
        }

        int64 toInt() const
        {
            int64 v = 0;
            intIf(&v);
            return v;
        }

        void fromInt(int64 v)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd()) mi->value.SetInt64(v);
            else
                _json->AddMember(StringRefType(_tag), v, this->allocator());
            _json->_changed = true;
        }
    };

    struct DoubleValue: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;

        DoubleValue(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;
        }
        
        void operator=(double v) { fromDouble(v); }
        operator double() const { return toDouble(); }

        bool doubleIf(double* vp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = (mi != _json->MemberEnd());
            if (res)
                *vp = mi->value.GetDouble();
            return res;
        }

        double toDouble() const
        {
            double v = 0;
            doubleIf(&v);
            return v;
        }

        void fromDouble(double v)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd()) mi->value.SetDouble(v);
            else
                _json->AddMember(StringRefType(_tag), v, this->allocator()); 
            _json->_changed = true;
        }
    };

    struct ArrayValue: public ValueBase
    {
        using ValueBase::_json;
        using ValueBase::_tag;
        typedef std::string string;

        const Value* _array;

        ArrayValue(Value& v)
        {
            assert(v.IsArray());
            _array = &v;
        }
        
        ArrayValue(JSONB& host, const char* tag)
        {
            _json = &host;
            _tag = tag;

            // look up the array immediately
            if (!arrayIf(&_array))
                _array = 0;
        }

        operator bool() const { return _array != 0; }
        
        bool arrayIf(const Value** vp) const
        {
            jLock lk(_json->_lock);
            ConstMemberIterator mi = _json->FindMember(_tag);
            bool res = (mi != _json->MemberEnd());
            if (res)
            {
                *vp = &mi->value;
                res = (*vp)->IsArray();
            }
            return res;
        }
        
        size_t size() const { return _array ? _array->Size() : 0; }

        const GenericValue& at(size_t i) const
        {
            // ASSUME valid
            return (*_array)[i];
        }

        /*
        GenericValue& at(size_t i) 
        {
            // ASSUME valid
            return (*_array)[i];
        }
        */
        
        string getString(int i) const
        {
            return _array ? at(i).GetString() : string();
        }

        /*
        void fromArray(array v)
        {
            jLock lk(_json->_lock);
            MemberIterator mi = _json->FindMember(_tag);
            if (mi != _json->MemberEnd()) mi->value.SetArray(v);
            else
                _json->AddMember(StringRefType(_tag), v, _json->GetAllocator()); 
            _json->_changed = true;
        }
        */
    };

    static JSONB* fromString(const char* s)
    {
        RSStream rs(s);
        
        JSONB* json = new JSONB;
        json->ParseStream<rapidjson::kParseStopWhenDoneFlag>(rs);

        if (json->HasParseError())
        {
            LOG("Parse Error: Offset:",
                (unsigned int)json->GetErrorOffset()
                << ' ' << rapidjson::GetParseError_En(json->GetParseError()));
            
            LOG2("JSON:\n", s);
            delete json; json = 0;
        }
        return json;
    }

    void flatten(rapidjson::StringBuffer& sb) const
    {
        jLock lk(_lock);
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        Accept(writer);
    }

    std::string toString() const
    {
        rapidjson::StringBuffer sb;
        flatten(sb);        
        return sb.GetString();
    }

    friend std::ostream& operator<<(std::ostream& os, const JSONB& js)
    {
        rapidjson::StringBuffer sb;
        js.flatten(sb);
        return os << sb.GetString();
    }


    static const Value& at(const Value& v, const char* tag)
    {
        ConstMemberIterator mi = v.FindMember(tag);
        if (mi != v.MemberEnd())
            return mi->value;
        else
        {
            static GenericValue NullValue;
            return NullValue;
        }   
    }

    static int toInt(const Value& v)
    {
        return v.IsNumber() ? v.GetInt() : 0;
    }

    static std::string toString(const Value& v) 
    {
        if (v.IsNull()) return std::string();
        if (v.IsString())
        {
            return v.GetString(); // dont include quotes
        }
        else
        {
            rapidjson::StringBuffer sb;        
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            v.Accept(writer);
            return sb.GetString();
        }
    }

};

// non-locking JSON data
typedef struct JSONB<LockDummy> JSON;

// thread safe version
typedef struct JSONB<std::mutex> JSONMT;


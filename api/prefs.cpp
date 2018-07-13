/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */


#include <chrono>
#include <future>
#include <mutex>
#include "imptypes.h"
#include "control.h"
#include "json.h"
#include "fd.h"
#include "jtrans.h"
#include "logged.h"
#include "utils.h" // to_string

struct Prefs::Imp : public ImpType<Prefs>
{
    Imp(Prefs* host) : ImpType(host) { _init(); }

    ~Imp() { delete _prefs; }

    JSONMT*                     _prefs;
    std::mutex                  _saveLock;
    int                         _saveDelay;
    std::future<bool>           _saveResult;

    bool        _load()
    { return _load(_host->_control->prefsFilePath().c_str()); }

    bool        _load(const char* name)
    {
        _prefs = 0;

        FD fd;
        if (fd.open(name))
        {
            size_t sz = fd.size();
            if (sz)
            {
                JTrans jt;
                jt.allocate(sz);
                if (fd.read((uchar*)jt._buf, sz))
                {
                    jt._buf[sz] = 0; 

                    // repair some cases
                    jt.fixJson();
                    _prefs = JSONMT::fromString(jt._buf);

                    if (_prefs)
                    {
                        LOG2("Prefs::loaded ", name);
                    }
                }
            }
        }

        if (!_prefs)
        {
            // initially there is no file
            LOG1("Prefs, failed to load '", name << '\'');

            // will create file when we write
            _prefs = new JSONMT;
            _prefs->SetObject();
        }

        // note: file is closed
        
        return _prefs != 0;
    }

    void        _changed()
    { 
        // trigger async save after a short delay
        saveAsync(); 
    }

    void saveAsync()
    {
        bool start;
        
        // increase delay or trigger save thread
        _saveLock.lock();
        start = _saveDelay++ == 0;
        if (_saveDelay > 2) _saveDelay = 2;
        _saveLock.unlock();

        if (start)
            _saveResult = std::async(std::launch::async, &Imp::_save, this);
    }

    bool _save()
    {
        // running async
        bool res = false;
        bool done = false;

        do
        {
            // saveDelay >= 1
            bool wait = true;
            do
            {
                std::this_thread::sleep_for( std::chrono::milliseconds(500));

                _saveLock.lock();
                if (_saveDelay > 1) --_saveDelay;
                else wait = false;  // delay == 1
                _saveLock.unlock();
            
            }  while (wait);

            // delay >= 1, most likely 1
            
            LOG2("prefs, ", "saving... " << _saveDelay); // std::this_thread::get_id());

            string fname = _host->_control->prefsFilePath();

            FD fd;
            if (fd.open(fname.c_str(), 
                        FD::fd_wr | FD::fd_create | FD::fd_trunc))
            {
                rapidjson::StringBuffer sb;
                if (_prefs)
                    _prefs->flatten(sb);
                else
                {
                    sb.Put('{');
                    sb.Put('}');
                }
                sb.Put('\n');

                if (!fd.write((unsigned char*)sb.GetString(), sb.GetSize()))
                {
                    LOG("prefs, ", "Write Error");
                }
                else
                {
                    res = true;
                    LOG2("prefs ", "wrote '" << fname << '\'');
                }
            }

            // delay >= 1

            _saveLock.lock();
            
            if (!res)
            {
                // if failed to save, don't retry
                _saveDelay = 0;
                done = true;
            }
            else
            {
                // eg if 2, will wait another 1 before save again
                // if 1, we're done
                done = --_saveDelay == 0;
            }
            _saveLock.unlock();
            
        } while (!done);
        
        return res;
    }

    bool remove(const char* key)
    {
        bool res = _prefs != 0;
        if (res)
        {
            res = _prefs->remove(key);
            if (res) _changed();
        }
        return res;
    }

private:

    void _init()
    {
        _saveDelay = 0;
    }
};

void Prefs::_init()
{
    _imp = new Imp(this);
    _imp->_load();
}

Prefs::~Prefs() { delete _imp; }

Prefs::string Prefs::getString(const char* key, const char* fallback) const
{
    const char* str = fallback;
    if (_imp->_prefs)
        JSONMT::StringValue(*_imp->_prefs, key).stringIf(&str);
    return str;
}

int Prefs::getInt(const char* key, int fallback) const
{
    int v = fallback;
    if (_imp->_prefs)
        JSONMT::IntValue(*_imp->_prefs, key).intIf(&v);
    return v;
}

double Prefs::getDouble(const char* key, double fallback) const
{
    double v = fallback;
    if (_imp->_prefs)
        JSONMT::DoubleValue(*_imp->_prefs, key).doubleIf(&v);
    return v;
}

bool Prefs::isDefined(const char* key) const
{
    return _imp->_prefs && _imp->_prefs->isDefined(key);
}

bool Prefs::remove(const char* tag) { return _imp->remove(tag); }

void Prefs::set(const char* key, const char* v)
{
    if (_imp->_prefs)
    {
        JSONMT::StringValue(*_imp->_prefs, key) = v;
        _imp->_changed();
    }
}

void Prefs::set(const char* key, int v)
{
    if (_imp->_prefs)
    {
        JSONMT::IntValue(*_imp->_prefs, key) = v;
        _imp->_changed();
    }
}

void Prefs::set(const char* key, double v)
{
    if (_imp->_prefs)
    {
        JSONMT::DoubleValue(*_imp->_prefs, key) = v;
        _imp->_changed();
    }
}

std::string Prefs::toString() const 
{
    return _imp->_prefs ? _imp->_prefs->toString() : string();
}


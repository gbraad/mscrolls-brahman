/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 *  Copyright (�) Voidware 2016-2017.
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

#include <map>
#include "var.h"

struct VarSet: public std::map<std::string, var>
{
    typedef std::map<std::string, var> parentT;
    typedef std::string string;
    
    void add(const string& key, const var& val)
    {
        // NB: consumes `val` !
        insert(std::make_pair(key, val));
    }

    var find(const string& key) const
    {
        var v;
        const_iterator it = parentT::find(key);
        if (it != end()) v = it->second.copy();
        return v;
    }

    friend std::ostream& operator<<(std::ostream& os, const VarSet& vs)
    {
        for (const_iterator it = vs.cbegin(); it != vs.cend(); ++it)
        {
            os << it->first << ':' << it->second << std::endl;
        }
        return os;
    }
    
};

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
 *  Copyright (c) Strand Games 2020.
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


#ifdef STANDALONE
#include <time.h>
#endif

#include "fd.h"
#include "strands.h"

namespace ST
{
Term::Terms Term::_allTerms(&Term::compareLess);
};

#ifdef STANDALONE

using namespace ST;

#include "strandi.h"
#include "pstrands.h"

static ParseStrands ps;

int main(int argc, char** argv)
{
    int debug = 0;

    std::vector<std::string> files;
    
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (!stricmp(argv[i], "-d"))
            {
                debug = 1;
                if (i < argc-1 && u_isdigit(*argv[i+1]))
                {
                    debug = atoi(argv[++i]);
                }
            }
        }
        else
        {
            files.push_back(argv[i]);
        }
    }

    if (!files.size())
    {
        printf("Usage: %s [-d] file...\n", argv[0]);
        return -1;
    }

    if (debug) printf("debug level %d\n", debug);
    ps._debug = debug;

    if (ps.loadFiles(files))
    {
        bool v = ps.validate();

        if (v && ps._startTerm)
        {
            Strandi si(&Term::_allTerms);
            si.setdebug(debug);

            time_t tt;
            si.randomise(time(&tt));

            if (!si.start(ps._startTerm))
            {
                ERR0("cannot start");
            }
        }
    }
    
    return 0;
}


/* BASIC GLUE */

void _emit(const char* s)
{
    printf("%s", s);
}

char _getchar()
{
    int c = getchar();
    return c;
}
    
#endif // STANDALONE

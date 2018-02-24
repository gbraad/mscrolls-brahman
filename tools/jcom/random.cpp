

#include <stdlib.h>
#include <sys/types.h>

#ifndef _WIN32
# include <sys/stat.h>
# include <sys/time.h>
#endif

#ifdef __linux__
# include <sys/syscall.h>
# include <poll.h>
#endif

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

#ifndef _WIN32
# include <unistd.h>
#else
#include <windows.h>
#endif

#include "random.h"

#ifdef _WIN32
#define RtlGenRandom SystemFunction036
# pragma comment(lib, "advapi32.lib")

extern "C"
BOOLEAN NTAPI RtlGenRandom(PVOID RandomBuffer, ULONG RandomBufferLength);

#endif // _WIN32

# if defined(SYS_getrandom) && defined(__NR_getrandom)
static int
_randombytes_linux_getrandom(void * const buf, const size_t size)
{
    int readnb;
    
    assert(size <= 256U);
    do {
        readnb = syscall(SYS_getrandom, buf, (int) size, 0);
    } while (readnb < 0 && (errno == EINTR || errno == EAGAIN));

    return (readnb == (int) size) - 1;
}

static int
randombytes_linux_getrandom(void * const buf_, size_t size)
{
    unsigned char *buf = (unsigned char *) buf_;
    size_t         chunk_size = 256U;

    do {
        if (size < chunk_size) {
            chunk_size = size;
            assert(chunk_size > (size_t) 0U);
        }
        if (_randombytes_linux_getrandom(buf, chunk_size) != 0) {
            return -1;
        }
        size -= chunk_size;
        buf += chunk_size;
    } while (size > (size_t) 0U);

    return 0;
}
# endif


bool randombytes_sysrandom_buf(void* buf, size_t size)
{
#ifndef _WIN32
    
# if defined(SYS_getrandom) && defined(__NR_getrandom)
    if (!randombytes_linux_getrandom(buf, size)) return true;
# endif
    
#else
    
    if (RtlGenRandom((PVOID) buf, (ULONG) size)) return true;
#endif

    return false;
}

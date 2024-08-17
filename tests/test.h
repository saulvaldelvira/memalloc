#ifndef __TEST_H__
#define __TEST_H__

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static int OK = 1;

static inline long get_time_millis(void){
	return clock() * 1000.0 / CLOCKS_PER_SEC;
}

#ifndef NO_COLOR
#define Color_Reset  "\033[0m"
#define Color_Green  "\033[0;32m"
#define Color_Yellow "\033[0;33m"
#define Color_BRed   "\033[1;31m"
#define Color_BCyan  "\033[1;36m"
#else
#define Color_Reset  ""
#define Color_Green  ""
#define Color_Yellow ""
#define Color_BRed   ""
#define Color_BCyan  ""
#endif

#define _(body) do { body; } while(0)

#define test_step(name) _( printf("* " Color_Yellow "%s" Color_Reset " ... ", name); fflush(stdout); )
#define test_ok() 	_( printf(Color_Green "OK\n" Color_Reset) )

#define test_start(name) printf(" TEST %s\n", name);

#define assert(expr) _( \
        if (!(expr)){ \
                fprintf(stderr, Color_BRed "[%d] ASSERT FAILED: %s" Color_Reset "\n", __LINE__, #expr); \
                OK = 0; \
                exit(1); \
        })

#endif /* __TEST_H__ */

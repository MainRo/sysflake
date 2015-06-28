/*
*****************************************************************************
*                      ___       _   _    _ _
*                     / _ \ __ _| |_| |__(_) |_ ___
*                    | (_) / _` | / / '_ \ |  _(_-<
*                     \___/\__,_|_\_\_.__/_|\__/__/
*                   Copyright (c) 2015 Romain Picard
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include "sysflake.h"

#define TIME_BIT_SIZE       37
#define TID_BIT_SIZE        17
#define TID_BIT_MASK        ((1<<TID_BIT_SIZE)-1)
#define COUNTER_BIT_SIZE    10
#define COUNTER_BIT_MASK    ((1<<COUNTER_BIT_SIZE)-1)

pid_t sysflake_gettid(void)
{
    return syscall(SYS_gettid);
}

int64_t sysflake_generate(void)
{
    static __thread uint16_t counter = 0;
    static __thread uint64_t current_ms = 0;

    struct timespec time;
    uint64_t time_ms = 0;
    pid_t tid;
    int64_t flake = 0;

    if(clock_gettime(CLOCK_MONOTONIC, &time) != 0)
        return 0;

    time_ms = (uint64_t)time.tv_sec * 1000;
    time_ms += time.tv_nsec / 1000000;

    if(time_ms == current_ms)
    {
        if(counter >= (1<<COUNTER_BIT_SIZE))
            return 0;
    }
    else
    {
        current_ms = time_ms;
        counter = 0;
    }

    tid = sysflake_gettid();

    flake = time_ms << (TID_BIT_SIZE+COUNTER_BIT_SIZE);
    flake += ((int64_t)tid & TID_BIT_MASK) << COUNTER_BIT_SIZE;
    flake += (counter & COUNTER_BIT_MASK);

    counter += 1;
    return flake;
}

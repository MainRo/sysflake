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

/*
   These test ensure that the implementation is conforming to the sysflake
   specifications, i.e. each part of the generated id is correct.
*/


#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "sysflake.h"
#include <time.h>

extern "C" {
pid_t sysflake_gettid(void);
}

#define TIME_BIT_SIZE       37
#define TIME_BIT_MASK       0x1FFFFFFFFF
#define TID_BIT_SIZE        17
#define TID_BIT_MASK        0x1FFFF
#define COUNTER_BIT_SIZE    10
#define COUNTER_BIT_MASK    0x3FF

int clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    mock().actualCall("clock_gettime")
        .withParameter("clk_id", clk_id)
        .withOutputParameter("tp", tp);
    return mock().intReturnValue();
}

pid_t sysflake_gettid(void)
{
    mock().actualCall("sysflake_gettid");
    return mock().intReturnValue();
}

TEST_GROUP(SpecTestCase)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }

    /* resets the counter associated to the current thread by issuing a flake
     * with a time not used by any test. This effectively resets the counter.
    */
    void resetCounter(void)
    {
        struct timespec mockTime = {1902,42000000};
        mock().expectOneCall("clock_gettime")
            .withParameter("clk_id", CLOCK_MONOTONIC)
            .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
            .andReturnValue(0);
        mock().expectOneCall("sysflake_gettid")
            .andReturnValue(0);
        sysflake_generate();
    }
};


TEST(SpecTestCase, TimeBitStream)
{
    struct timespec mockTime = {10,42000000};

    resetCounter();
    mock().expectOneCall("clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(0);
    mock().expectOneCall("sysflake_gettid")
        .andReturnValue(0);

    int64_t flake = sysflake_generate();
    int64_t flakeMs = (flake >> (TID_BIT_SIZE + COUNTER_BIT_SIZE)) & TIME_BIT_MASK;
    struct timespec flakeTs;

    flakeTs.tv_sec = flakeMs / 1000;
    flakeTs.tv_nsec = flakeMs % 1000;
    flakeTs.tv_nsec *= 1000000;
    CHECK_EQUAL(mockTime.tv_sec, flakeTs.tv_sec);
    CHECK_EQUAL(mockTime.tv_nsec, flakeTs.tv_nsec);
}

TEST(SpecTestCase, TidBitStream)
{
    struct timespec mockTime = {0,0};
    pid_t tid = TID_BIT_MASK;

    resetCounter();
    mock().expectOneCall("clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(0);
    mock().expectOneCall("sysflake_gettid")
        .andReturnValue(tid);

    int64_t flake = sysflake_generate();
    pid_t flakeTid = (flake >> COUNTER_BIT_SIZE) & TID_BIT_MASK;
    CHECK_EQUAL(tid, flakeTid);
}

TEST(SpecTestCase, Counter)
{
    struct timespec mockTime = {0,0};
    pid_t tid = TID_BIT_MASK;

    resetCounter();

    mock().expectNCalls(1<<COUNTER_BIT_SIZE, "clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(0);
    mock().expectNCalls(1<<COUNTER_BIT_SIZE, "sysflake_gettid")
        .andReturnValue(tid);

    for(int i=1; i<(1<<COUNTER_BIT_SIZE); i++)
    {
        sysflake_generate();
    }

    int64_t flake = sysflake_generate();
    uint32_t counter = flake & COUNTER_BIT_MASK;
    CHECK_EQUAL(COUNTER_BIT_MASK, counter);

    // next one is an overflow and must return 0
    mock().expectOneCall("clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(0);

    flake = sysflake_generate();
    CHECK_EQUAL(0, flake);
}

TEST(SpecTestCase, TimeMax)
{
    uint64_t maxMs = ((uint64_t)1 << TIME_BIT_SIZE) - 1;
    uint64_t maxSec = maxMs / 1000;
    uint64_t maxNs = maxMs % 1000 * 1000000;

    struct timespec mockTime = {maxSec,maxNs};

    resetCounter();
    mock().expectOneCall("clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(0);
    mock().expectOneCall("sysflake_gettid")
        .andReturnValue(0);

    int64_t flake = sysflake_generate();
    int64_t flakeMs = (flake >> (TID_BIT_SIZE + COUNTER_BIT_SIZE)) & TIME_BIT_MASK;
    CHECK_EQUAL(maxMs, (uint64_t)flakeMs);
}

TEST(SpecTestCase, GetTimeError)
{
    struct timespec mockTime = {10,42000000};

    resetCounter();
    mock().expectOneCall("clock_gettime")
        .withParameter("clk_id", CLOCK_MONOTONIC)
        .withOutputParameterReturning("tp", &mockTime, sizeof(mockTime))
        .andReturnValue(-1);

    int64_t flake = sysflake_generate();
    CHECK_EQUAL(0, flake);
}

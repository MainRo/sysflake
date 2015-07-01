# SysFlake

Sysflake is a library that can be used to generate a system unique id. This
library can be used from command line via the "sysflake" tool, or from c code.

Sysflake is inspired from simpleflake, but adapted to generate runtime ids on
standalone linux systems. It runs in a library embedded in each process that
needs to generate ids. No deamon is running. Sysflake can generate 1024 unique
ids per thread per millisecond for more than 4 years.

## When Can I Use Sysflake ?

If you need to generate a reasonable number of runtime unique ids on your system
then sysflake can be a solution.

## When Should I NOT Use Sysflake ?

If you need persitant unique ids, unique ids between several devices, ids that
must be hard to guess form a security point of view, then sysflake is not the
tool you need.

if you need to generate more than 1024 ids per millisecond from the same thread
then sysflake is not the tool you need.

If you increased the default value of "/proc/sys/kernel/pid_max" on a 64 bits
linux kernel, sysflake does not work.

# Specification

A flake is encoded as a 64 bits signed integer (int64_t). Its content is built
from the following information:

    +-----------+--------+---------+
    | timestamp | thread | counter |
    +-----------+--------+---------+

- timestamp is a 37 bits milli-seconds time.
- thread is the 17 bits linux thread id.
- counter is a 10 bits counter to ensure id uniqueness within a milli-second.

# Usage

## From Command Line

you can use sysflake from script via the sysflake tool:

    $ sysflake
    002906A15CABF400

## From C Code

    #include "sysflake.h"

    void foo(void)
    {
        int64_t flake = sysflake_generate();
    }

In case or error, sysflake_generate returns 0.

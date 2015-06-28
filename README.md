# SysFlake

Sysflake is a library that can be used to generate a system unique id. This
library can be used from command line via the "sysflake" tool, or from c code.

Sysflake is inspired form simpleflake, but adapted to runtime standalone linux
systems. It runs in a library embedded in each process that needs to generate
ids. No deamon is running. Sysflake can generate 1024 unique ids per thread per
millisecond for more than 8 years. 

## When Can I Use Sysflake ?

If you need to generate a reasonable number of runtime unique ids on your system
then sysflake can be a solution. Sysl

## When Should I NOT Use Sysflake ?

If you need persitant unique ids, unique ids accross several devices, ids that
must be hard to guess form a security point of view, then sysflake is not the
tool you need.

if you need to generate more than 1024 id per millisecond from the same thread
then sysflake is not the tool you need.

If you increased the default value of "/proc/sys/kernel/pid_max" on a 64 bits
linux kernel, sysflake does not work.

# Specification

a flake is encoded as a 64 bits signed integer (int64_t). Its content is built
from the following information:

    +-----------+--------+---------+
    | timestamp | thread | counter |
    +-----------+--------+---------+

- timestamp is a 37 bits milli-seconds time.
- thread is the 17 bits linux thread id.
- counter is a 10 bits counter to ensure id uniqueness within a milli-second.


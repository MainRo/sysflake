
check_PROGRAMS = utest-sysflake utest-sysflake-spec
utest_sysflake_SOURCES = main.cpp TestGenerate.cpp
utest_sysflake_CPPFLAGS = -I../lib
utest_sysflake_CPPFLAGS += $(CPPUTEST_CFLAGS)
utest_sysflake_LDADD   = $(CPPUTEST_LIBS) $(top_srcdir)/lib/libsysflake.la

utest_sysflake_spec_SOURCES = main.cpp TestSpec.cpp
utest_sysflake_spec_CPPFLAGS = -I../lib
utest_sysflake_spec_CPPFLAGS += $(CPPUTEST_CFLAGS)
utest_sysflake_spec_LDADD   = $(CPPUTEST_LIBS) $(top_srcdir)/lib/libsysflake.la

TESTS = $(check_PROGRAMS)


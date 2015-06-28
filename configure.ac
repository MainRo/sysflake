AC_PREREQ([2.68])
AC_INIT([sysflake], [1.0.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([lib/flake.c])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR

AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/param.h unistd.h stdbool.h])

# check for libraries
PKG_CHECK_MODULES([CPPUTEST], [cpputest >= 3.6], 
      [have_cpputest=yes], [have_cpputest=no])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_INLINE
#AC_TYPE_SIZE_T
#AC_TYPE_SSIZE_T

# Checks for library functions.
#AC_CHECK_FUNCS([getcwd memset mkdir strcasecmp strchr strdup])


AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile test/Makefile])
AM_CONDITIONAL([COND_CPPUTEST], [test "x${have_cpputest}" = xyes])

AC_OUTPUT

echo "
sysflake ($PACKAGE_NAME) version $PACKAGE_VERSION
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS"

if test "x${have_cpputest}" = xyes; then
   echo "CppUTest.......: $CPPUTEST_LIBS"
else
   echo "CppUTest.......: Disabled"
fi

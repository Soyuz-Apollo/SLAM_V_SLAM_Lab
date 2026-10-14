# Process this file with autoconf to produce a configure script.

AC_INIT(Scene, 0.9, ajd@doc.ic.ac.uk, scenelib)
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR([Scene/scene_single.h])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE
AC_CANONICAL_HOST

# By default we install the headers and the 
# libraries in the same place as the source
AC_PREFIX_DEFAULT(`pwd`)

# Do we want debug or optimised?
AC_DEBUG_OR_OPTIMISE_CXXFLAGS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.
AC_FIND_VW(VWGL)

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile
                 Scene/Makefile
                 SceneImproc/Makefile
                 MonoSLAM/Makefile])
AC_OUTPUT

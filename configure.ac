#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([fontgen], [m4_esyscmd_s([git describe --always --tag --dirty])], [Tobias_Mueller@twam.info])

# prepare for automake
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([options.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([freetype], [FT_Init_FreeType])
AC_CHECK_LIB([iconv], [iconv_open])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strdup strtoul])

AC_OUTPUT

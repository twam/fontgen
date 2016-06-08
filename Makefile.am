AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip foreign

AC_CHECK_HEADER([iconv.h], ,[AC_MSG_ERROR([can not find iconv.h])])
AC_CHECK_LIB([iconv], [iconv_open], ,[AC_MSG_ERROR([can not find iconv_open])])

CFLAGS = -Wall -std=c99 -O2 `freetype-config --cflags` -g -D_GNU_SOURCE
LDFLAGS = `freetype-config --libs`

bin_PROGRAMS = fontgen
fontgen_SOURCES = character.c charsets.c convert.c font.c glyph.c main.c options.c output.c output_bmp.c output_twam.c


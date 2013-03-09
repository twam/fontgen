AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip foreign

CFLAGS = -Wall -std=c99 -O2 `freetype-config --cflags` -g
LDFLAGS = `freetype-config --libs`

bin_PROGRAMS = fontgen
fontgen_SOURCES = character.c charsets.c convert.c font.c glyph.c main.c  options.c output_twam.c


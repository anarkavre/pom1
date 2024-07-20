AC_PREREQ([2.68])
AC_INIT([Pom1 Apple 1 Emulator], [1.0.0], [jdcorrado@gmail.com], [pom1])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

PACKAGE_SHORTDESC="Apple 1 emulator"
PACKAGE_COPYRIGHT="Copyright (C) 2000-2012"
PACKAGE_LICENSE="GNU General Public License, version 2"
PACKAGE_MAINTAINER="John D. Corrado"
PACKAGE_URL="http://pom1.sourceforge.net/"

AC_PROG_CC
AC_PROG_INSTALL

AC_CHECK_HEADERS([stdlib.h string.h])

AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit memset mkdir strcasecmp strdup strrchr])

AM_PATH_SDL([1.1.3])

CFLAGS="$CFLAGS $SDL_CFLAGS"
LDFLAGS="$LDFLAGS $SDL_LIBS"

AM_INIT_AUTOMAKE([1.8.0])

AC_SUBST([PACKAGE_SHORTDESC])
AC_SUBST([PACKAGE_COPYRIGHT])
AC_SUBST([PACKAGE_LICENSE])
AC_SUBST([PACKAGE_MAINTAINER])
AC_SUBST([PACKAGE_URL])

AC_CONFIG_FILES([
Makefile
src/Makefile
src/pom1.desktop
src/roms/Makefile
src/pom1
])
AC_OUTPUT

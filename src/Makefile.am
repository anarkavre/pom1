EXEEXT=-@PACKAGE_VERSION@
bin_PROGRAMS = pom1
bin_SCRIPTS = pom1

SOURCE_FILES =						\
	configuration.c		configuration.h		\
	keyboard.c		keyboard.h		\
	m6502.c			m6502.h			\
	main.c						\
	memory.c		memory.h		\
	options.c		options.h		\
	pia6820.c		pia6820.h		\
	screen.c		screen.h

pom1_SOURCES = $(SOURCE_FILES)
pom1_LDADD = @LDFLAGS@

EXTRA_DIST = pom1.png

appdir = $(prefix)/share/applications
app_DATA = pom1.desktop

iconsdir = $(prefix)/share/icons
icons_DATA = pom1.png

SUBDIRS = roms
DIST_SUBDIRS = roms

# dwm - dynamic window manager
# See LICENSE file for copyright and license details.

include config.mk

SRC1 = drw.c dwm.seat1.c util.c
SRC2 = drw.c dwm.seat2.c util.c
OBJ1 = ${SRC1:.c=.o}
OBJ2 = ${SRC2:.c=.o}

all: options dwm.seat1 dwm.seat2

options:
	@echo dwm build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ1}: config.seat1.h config.mk
${OBJ2}: config.seat2.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

dwm.seat1: ${OBJ1}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ1} ${LDFLAGS}

dwm.seat2: ${OBJ2}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ2} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f dwm.seat* ${OBJ1} ${OBJ2} dwm-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p dwm-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk \
		dwm.1 ${SRC1} ${SRC2} dwm-${VERSION}
	@tar -cf dwm-${VERSION}.tar dwm-${VERSION}
	@gzip dwm-${VERSION}.tar
	@rm -rf dwm-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f dwm.seat* ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/dwm.seat*
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < dwm.1 > ${DESTDIR}${MANPREFIX}/man1/dwm.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/dwm.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/dwm.seat*
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/dwm.1

.PHONY: all options clean dist install uninstall

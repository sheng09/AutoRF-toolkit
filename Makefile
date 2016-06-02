export HOME=/home/sheng/ProgramSheng/Mylib/AutoRF

SUBS = src

INSTALL = $(foreach sub, ${SUBS}, ${sub}.install)
CLEAN   = $(foreach sub, ${SUBS}, ${sub}.clean)


all: ${SUBS}
	make -C $<

install: ${INSTALL}

clean: ${CLEAN}

%.install: %
	make install -C $<

%.clean: %
	make clean -C $<


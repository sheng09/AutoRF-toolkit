bindir=${HOME}/bin
libdir=${HOME}/lib

CC = gcc
AR = ar
CFLAGS = -Ofast -I../include -L.
OBJ = liberrmsg.o  libnumrec.o  sacio.o autorf.o 
LIB = libautorf.a
EXEC = pickRf pickRf_WV

all: ${OBJ} ${LIB} ${EXEC}

${OBJ}:%.o:%.c
	${CC} -c -o $@ $< ${CFLAGS}

${LIB}:%.a:${OBJ}
	${AR} cq $@ $^
	ranlib $@

${EXEC}:%:%.c
	${CC} -o $@ $< -lautorf -lm ${CFLAGS}

install:
	-mkdir ${bindir} ${libdir}
	cp ${EXEC} ${bindir}
	cp ${LIB}  ${libdir}

clean:
	rm ${OBJ} ${LIB} ${EXEC} -rf
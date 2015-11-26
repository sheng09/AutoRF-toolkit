CC  = gcc

SRC = ./src
BIN = ./bin
#LIB = ./lib
INC = ./include
OBJ = sacio.o autorf.o
OBJLIB =
OBJEXE = pickRf pickRf_WV

#CFLAGS = -lm -I./include -I${SFWHOME}/include -L./lib -L${SFWHOME}/lib -L. -Ofast -Wall
CFLAGS = -lm -Ofast -Wall -I./include
VPATH = ${SRC} ${INC}
#VPATH = ${SRC}:${INC}:${LIB}:${SFWHOME}/include:${SFWHOME}/lib

.PHONY:all


all: ${OBJ} ${OBJLIB} ${OBJEXE}


install: ${OBJLIB} ${OBJEXE}
	mkdir ${BIN} 2>&- || rm ${BIN}/* -rf 2>&-
	mv ${OBJEXE} ${BIN}
	#mkdir ${LIB} 2>&- || rm ${LIB}/* -rf 2>&-
	#mv ${OBJLIB} ${LIB}
	rm *.o


sacio.o: sacio.c sac.h
	${CC} -c ${SRC}/sacio.c ${CFLAGS}


autorf.o: autorf.c sac.h autorf.h
	#echo ${VPATH}
	${CC} -c ${SRC}/autorf.c  ${CFLAGS}
	#-lnumrec ${CFLAGS}


pickRf: pickRf.c autorf.h sac.h libnumrec.h autorf.o sacio.o libnumrec.o liberrmsg.o
	${CC} -o pickRf ${SRC}/pickRf.c autorf.o sacio.o libnumrec.o liberrmsg.o ${CFLAGS}


pickRf_WV:pickRf_WV.c autorf.h sac.h libnumrec.h autorf.o libnumrec.o liberrmsg.o
	${CC} -o pickRf_WV ${SRC}/pickRf_WV.c autorf.o sacio.o libnumrec.o liberrmsg.o ${CFLAGS}


.PHONY:clean

clean:
	-rm ${OBJ} ${OBJLIB} ${OBJEXE} ${LIB} ${BIN} *.o -rf 2>&-


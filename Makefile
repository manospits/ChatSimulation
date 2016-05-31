PTHREAD     = -pthread
CC          = gcc
CRYPTOFLAG  = -lcrypto
OPENSSLFLAG = -lssl
SOURCES     = chat.c threads.c
HEADERS     = threads.h
FLAGS       = ${OPENSSLFLAG} ${CRYPTOFLAG} ${PTHREAD} -Wall

chat: chat.o
	${CC} $^ threads.o ${FLAGS} -o $@

chat.o: threads.o
	${CC} -c chat.c -o $@

threads.o : threads.c
	${CC} -c $^ -o $@

clean:
	\rm -f chat.o threads.o chat

count:
	wc ${HEADERS} ${SOURCES}

all:
	gcc -o sock sock.c

clean:
	rm -f sock.o sock

install: all
	cp sock /sbin/socketd

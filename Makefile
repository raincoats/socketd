all:
	gcc -o sock sock.c

clean:
	rm -f sock.o sock

install: all
	cp sock /sbin/socketd

uninstall:
	rm -f /sbin/socketd

debug:
	gcc -o sock sock.c -DDEBUG

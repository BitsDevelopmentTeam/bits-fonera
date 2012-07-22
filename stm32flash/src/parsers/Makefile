
all:
	$(CC) -Os -s -Wall -c -I../ binary.c hex.c
	$(AR) r parsers.a        binary.o hex.o

clean:
	rm -f *.o parsers.a

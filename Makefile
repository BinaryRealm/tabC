all: tabc
tabc: tabc.c
	gcc -o tabc tabc.c
.PHONY: clean all
clean:
	rm tabc

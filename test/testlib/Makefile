CC=gcc
FLAGS= -fPIC -shared -Wall


all:
	$(shell test -d ../../out/test/testlib || mkdir ../../out/test/testlib )
	$(CC) $(FLAGS) test.c -o ../../out/test/testlib/libtest.so

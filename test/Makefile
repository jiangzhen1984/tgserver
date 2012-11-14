CC=gcc
FLAGS= -DLINUX
TEST=../out/test/

	
all:service_test context_test util_test server_test event_test process_response_test client_test client_test1 client_test2 performace_test process_reader_test

service_test:test.so
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

context_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

server_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

util_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

event_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

process_response_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -lcontext -L../ -Ltestlib/ -ltest -ldl && rm *.o

client_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@  && rm *.o

client_test1:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@  && rm *.o

client_test2:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@  && rm *.o

performace_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@  && rm *.o

process_reader_test:
	$(CC) $(FLAGS) -c $@.c -o $(TEST)/$@.o 
	cd $(TEST)/ && $(CC) $@.o -o $@ -L../ -lcontext && rm *.o

test.so:
	make -C testlib/


clean:
	rm $(TEST)/service_test $(TEST)/context_test $(TEST)/util_test $(TEST)/server_test $(TEST)/event_test $(TEST)/process_response_test

test.so:
	make -C testlib/


clean:
	rm $(TEST)/service_test $(TEST)/context_test $(TEST)/util_test $(TEST)/server_test $(TEST)/event_test $(TEST)/process_response_test


CC=gcc 
FLAGS= -g -Wall -DLINUX -DDEBUG 
OUTPUT=out/
TESTDIR=$(OUTPUT)/test
PLUGIN=$(OUTPUT)/plugins




all:context plugin testcase
	$(CC) $(FLAGS) -c  main.c -o $(OUTPUT)/tgserver.o 
	cd $(OUTPUT) && $(CC) *.o -o tgserver -L. -lcontext -ldl && rm *.o
	cp -R bin/ $(OUTPUT)/
	mv $(OUTPUT)/tgserver $(OUTPUT)/bin/
	cp -R conf $(OUTPUT)/


context:
	if test -d $(OUTPUT); then echo "dir exists"; else mkdir $(OUTPUT); fi
	$(CC) $(FLAGS) tg_context.c tg_service.c tg_server.c tg_log.c tg_util.c tg_event.c tg_process.c tg_process_response.c tg_process_reader.c tg_process_admin.c -shared -fPIC -Wall -lpthread -ldl -o $(OUTPUT)/libcontext.so

plugin:
	if test -d $(PLUGIN); then echo "dir exists"; else mkdir $(PLUGIN); fi
	make -C plugins/

testcase:
	if test -d $(TESTDIR); then echo "dir exists"; else mkdir $(TESTDIR); fi
	make -C test/

clean:
	rm -rf out 


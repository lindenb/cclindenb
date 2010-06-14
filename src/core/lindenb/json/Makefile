
compile:lex.json_.o json.tab.o json_api.o datastream.o



json.tab.o:json.tab.c 
	$(CC) -o $@ -c $(CFLAGS) json.tab.c

json.tab.c json.tab.h: json.y json.h
	bison  json.y
	
lex.json_.o:lex.json_.c json.tab.h
	$(CC) -o $@  -c $(CFLAGS) lex.json_.c

lex.json_.c: json.l json.h json.y
	flex  json.l


clean:
	rm -f json.tab.c json.tab.h  lex.json_.c *.o a.out

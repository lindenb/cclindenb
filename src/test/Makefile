CFLAGS=-Wall -I ../core `xml2-config  --cflags` `curl-config  --cflags`
LDFLAGS=` xml2-config  --libs` `curl-config  --libs ` -lm
all:tests

tests:test1

test1:
	g++ $(CFLAGS) -o test0001.out test_0001.cpp $(LDFLAGS)
	./test0001.out
	rm ./test0001.out

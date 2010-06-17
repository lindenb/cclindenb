CFLAGS=-I ../core `xml2-config  --cflags` `curl-config  --cflags`
LDFLAGS=` xml2-config  --libs` `curl-config  --libs`
all:tests

tests:
	g++ $(CFLAGS) -o test0001.out test_0001.cpp $(LDFLAGS)
	./test0001.out
	rm ./test0001.out

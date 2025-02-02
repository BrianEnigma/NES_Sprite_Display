all: png2json

png2json: png2json.cpp
	g++ -std=c++11 -I/usr/local/include -L/usr/local/lib -lc -lgd -o png2json png2json.cpp

clean:
	rm -f png2json


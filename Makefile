all: tradgen

clean:
	rm tradgen

tradgen: tradgen.cpp
	g++ -ggdb tradgen.cpp -o tradgen

.PHONY: all clean

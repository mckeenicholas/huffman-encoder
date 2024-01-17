CFLAGS = -Wall -Werror -O3 -std=c++20

all: huffman_encoder

huffman_encoder: huffman_encoder.o compress.o expand.o
	g++ $(CFLAGS) -o huffman_encoder compress.o huffman_encoder.o expand.o

%.o: %.cpp %.h
	g++ $(CFLAGS) -c $<

clean:
	rm huffman_encoder *.o

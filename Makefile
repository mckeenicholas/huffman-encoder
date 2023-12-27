CFLAGS = -Wall -Werror -g

all: huffman_encoder

huffman_encoder: huffman_encoder.o compress.o expand.o
	g++ $(CFLAGS) -o huffman_encoder compress.o huffman_encoder.o expand.o

%.o: %.cpp %.h
	g++ $(CFLAGS) -c $<

clean:
	rm huffman_encoder *.o
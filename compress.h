#ifndef COMPRESS_H
#define COMPRESS_H

#include <map>
#include <fstream>

#include "huffman_encoder.h"

struct CompareNodes {
    bool operator()(const node* a, const node* b) const {
        return a->freq > b->freq;
    }
};

int compress_file(std::string& inputFileName, std::string& outputFileName, bool verbose);

node* buildHuffmanTree(const std::map<char, node*>& charMap);

std::map<char, node*> countCharacters(std::ifstream& inputFile);

void writeToBinary(const std::string& inputFile, const std::string& outputFile, std::map<char, std::string>& huffmanCodes,  node *huffmanTree);

void generateHuffmanCodes(node* root, const std::string& code, std::map<char, std::string>& huffmanCodes);

void postOrderTraversal(node *root, bool mode, std::string& output);

void padEnd(std::string& input);

#endif // COMPRESS_H
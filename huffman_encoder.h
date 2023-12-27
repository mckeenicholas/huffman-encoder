#ifndef HUFFMAN_ENCODER_H
#define HUFFMAN_ENCODER_H

#include <fstream>
#include <map>

struct node {
    int freq;
    char letter;
    struct node* left;
    struct node* right;
};

void printCharacterFrequencies(const std::map<char, node*>& charMap);

void printHuffmanTree(node* root, std::string code = "");

void printHelpMessage();

bool parseArguments(int argc, char** argv, std::string& inputFile, std::string& outputFile, bool& verbose, bool& mode);

#endif // HUFFMAN_ENCODER_H
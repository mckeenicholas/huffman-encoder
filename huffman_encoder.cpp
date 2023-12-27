#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <bitset>
#include <cstring>

#include "huffman_encoder.h"
#include "compress.h"

int main(int argc, char** argv) {
    std::string inputFileName, outputFileName;
    bool verbose = false;

    if (!parseArguments(argc, argv, inputFileName, outputFileName, verbose)) {
        return 1; // Exit with an error code
    }

    compress_file(inputFileName, outputFileName, verbose);

    return 0;
}

void printCharacterFrequencies(const std::map<char, node*>& charMap) {
    std::cout << "Character Frequencies:" << std::endl;
    for (const auto& pair : charMap) {
        std::cout << "Character: \"" << pair.first << "\" Frequency: " << pair.second->freq << std::endl;
    }
}

void printHuffmanTree(node* root, std::string code) {
    if (root) {
        if (root->letter != '\0') {
            // Leaf node, print the character and its code
            std::cout << "Character: \"" << root->letter << "\" Code: " << code << std::endl;
        }

        // Recursively traverse left and right subtrees
        printHuffmanTree(root->left, code + "0");
        printHuffmanTree(root->right, code + "1");
    }
}

void printHelpMessage() {
    std::cout << "Usage: ./compressor [-o output_file] [-h] [-v] input_file\n";
    std::cout << "-o output_file   Specify the output file for compressed data (default: a.out).\n";
    std::cout << "-h               Print this help message.\n";
    std::cout << "-v               Enable verbose output.\n";
}

bool parseArguments(int argc, char** argv, std::string& inputFile, std::string& outputFile, bool& verbose) {
    if (argc < 2) {
        printHelpMessage();
        return false;
    }

    outputFile = "a.out"; // Set default output file name

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-o") == 0) {
            // Check if there is an argument following -o
            if (i + 1 < argc) {
                outputFile = argv[i + 1];
                ++i; // Skip the next argument since it's the output file
            } else {
                std::cerr << "Error: Missing argument after -o.\n";
                printHelpMessage();
                return false;
            }
        } else if (std::strcmp(argv[i], "-h") == 0) {
            printHelpMessage();
            return false; // Indicate that the program should exit without processing further
        } else if (std::strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else {
            // Assume it's the input file
            inputFile = argv[i];
        }
    }

    return true;
}
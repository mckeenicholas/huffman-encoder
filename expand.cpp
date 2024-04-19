#include <iostream>
#include <bitset>

#include "huffman_encoder.h"
#include "expand.h"

int expand_file(std::string& inputFileName, std::string& outputFileName, bool verbose) {
    std::ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        perror("open");
        return 1;
    }

    // Read the length of the encoded data
    uint32_t length;
    inputFile.read(reinterpret_cast<char*>(&length), sizeof(length));

    // Read the size of the tree
    uint32_t treeSize;
    inputFile.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));

    std::string encodedData;
    std::bitset<8> bits;
    for (uint32_t i = 0; i < length; i += 8) {
        bits = inputFile.get();
        encodedData += bits.to_string();
    }

    // Read the leaf indicator
    std::string leafIndicator;  
    for (uint32_t i = 0; i < treeSize; i += 8) {
        bits = inputFile.get();
        leafIndicator += bits.to_string();
    }

    // Remove extra padding on binary data
    encodedData.erase(encodedData.begin() + length, encodedData.end());
    leafIndicator.erase(leafIndicator.begin() + treeSize, leafIndicator.end());

    // Read tree contents
    std::string treeValues;
    char c;
    while (inputFile.get(c)) {
        treeValues += c;
    }

    inputFile.close();

    node *huffmanRoot = reconstructTree(leafIndicator, treeValues);

    if (verbose) {
        printHuffmanTree(huffmanRoot);
    }

    std::string message = "";
    size_t index = 0;

    while (index < encodedData.length()) {
        node *current = huffmanRoot;
        while (current->left || current->right) {
            if (encodedData[index] == '0') {
                current = current->left;
            } else {
                current = current->right;
            }
            index++;
        }
        message += current->letter;
    }

    std::ofstream outputFile(outputFileName);

    if (!outputFile.is_open()) {
        perror("open");
        return 1;
    }

    outputFile << message;

    outputFile.close();

    return 0;
}

// Function to reconstruct Huffman tree
node *reconstructTree(std::string& structure, std::string& values) {
    if (structure.back() == '0') {
        node *root = new node{0, '\0', nullptr, nullptr};

        structure.pop_back();
        root->right = reconstructTree(structure, values);
        structure.pop_back();
        root->left = reconstructTree(structure, values);

        return root;
    } else {
        node *root = new node{0, values.back(), nullptr, nullptr};
        values.pop_back();
        return root;
    }
}
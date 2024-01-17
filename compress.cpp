#include <map>
#include <queue>
#include <bitset>
#include <iostream>
#include <iomanip>

#include "compress.h"

int compress_file(std::string& inputFileName, std::string& outputFileName, bool verbose) {
    std::ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        perror("open");
        return 1;
    }

    // Get the original file size
    inputFile.seekg(0, std::ios::end);
    size_t originalFileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::map<char, node*> charMap = countCharacters(inputFile);

    // Build Huffman tree
    node* huffmanRoot = buildHuffmanTree(charMap);

    // Generate Huffman codes
    std::map<char, std::string> huffmanCodes;
    generateHuffmanCodes(huffmanRoot, "", huffmanCodes);

    if (verbose) {
        printCharacterFrequencies(charMap);
        printHuffmanTree(huffmanRoot);
    }

    // Write Huffman-encoded data to binary file
    writeToBinary(inputFileName, outputFileName, huffmanCodes, huffmanRoot);

    // Free allocated memory for nodes
    for (auto& pair : charMap) {
        delete pair.second;
    }

    // Get the compressed file size
    std::ifstream compressedFile(outputFileName, std::ios::binary);
    compressedFile.seekg(0, std::ios::end);
    size_t compressedFileSize = compressedFile.tellg();
    compressedFile.close();

    // Calculate and print the percent file size reduction
    double reductionPercentage = (1.0 - static_cast<double>(compressedFileSize) / originalFileSize) * 100.0;
    std::cout << "Original file size: " << originalFileSize << ", compressed file size: " << compressedFileSize << " Reduction: " << std::fixed << std::setprecision(2) << reductionPercentage << "%" << std::endl;

    return 0;
}

node* buildHuffmanTree(const std::map<char, node*>& charMap) {
    // Use a priority queue to build the Huffman tree
    std::priority_queue<node*, std::vector<node*>, CompareNodes> pq;

    // Initialize the priority queue with nodes from the character frequency map
    for (const auto& pair : charMap) {
        pq.push(pair.second);
    }

    // Build the Huffman tree by merging nodes until there is only one node left
    while (pq.size() > 1) {
        node* left = pq.top();
        pq.pop();
        node* right = pq.top();
        pq.pop();

        // Create a new node with a frequency equal to the sum of the frequencies of its children
        node* mergedNode = new node{left->freq + right->freq, '\0', left, right};
        pq.push(mergedNode);
    }

    // The last node in the priority queue is the root of the Huffman tree
    return pq.top();
}

std::map<char, node*> countCharacters(std::ifstream& inputFile) {
    std::map<char, node*> charMap;
    char ch;

    while (inputFile.get(ch)) {
        if (charMap.find(ch) == charMap.end()) {
            // If character is not in the map, add it with frequency 1
            node* newNode = new node{1, ch, nullptr, nullptr};
            charMap[ch] = newNode;
        } else {
            // If character is already in the map, increment its frequency
            charMap[ch]->freq++;
        }
    }

    return charMap;
}

void generateHuffmanCodes(node* root, const std::string& code, std::map<char, std::string>& huffmanCodes) {
    if (root) {
        if (root->letter != '\0') {
            // Leaf node, store the character and its code in the map
            huffmanCodes[root->letter] = code;
        }

        // Recursively traverse left and right subtrees
        generateHuffmanCodes(root->left, code + "0", huffmanCodes);
        generateHuffmanCodes(root->right, code + "1", huffmanCodes);
    }
}

void padEnd(std::string& input) {
    size_t padding = 8 - (input.length() % 8);
    if (padding != 8) {
        input += std::string(padding, '0');
    }
}

void writeToBinary(const std::string& inputFile, const std::string& outputFile, std::map<char, std::string>& huffmanCodes, node *huffmanTree) {
    std::ifstream input(inputFile, std::ios::binary);
    std::ofstream output(outputFile, std::ios::binary);

    if (!input.is_open() || !output.is_open()) {
        perror("open");
        exit(1);
    }

    char ch;
    std::string encodedData;

    // Read input file character by character and append corresponding Huffman code to the encodedData string
    while (input.get(ch)) {
        encodedData += huffmanCodes[ch];
    }

    std::string leafIndicator, treeValues;
    postOrderTraversal(huffmanTree, true, leafIndicator);
    postOrderTraversal(huffmanTree, false, treeValues);

    // Write the length of the encoded data to the output file
    uint32_t length = encodedData.size();
    output.write(reinterpret_cast<const char*>(&length), sizeof(length));

    // Write the size of the tree to the output file
    uint32_t treeSize = leafIndicator.length();
    output.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));

    // Pad the end of encodedData and leafIndicator to they are a multiple of 8 bits
    padEnd(leafIndicator);
    padEnd(encodedData);

    // Convert binary string of encoded data to actual binary data and write to the output file
    std::bitset<8> bits;
    for (size_t i = 0; i < length; i += 8) {
        for (size_t j = 0; j < 8 && i + j < length; ++j) {
            // Using 7 - j for little endian order
            bits[7 - j] = (encodedData[i + j] == '1');
        }
        output.put(bits.to_ulong());
    }

    input.close();

    // Convert binary string of leafIndicator to binary and write to output file
    for (size_t i = 0; i < treeSize; i += 8) {
        for(size_t j = 0; j < 8 && i + j < treeSize; ++j) {
            // Using 7 - j for little endian order
            bits[7 - j] = (leafIndicator[i + j] == '1');
        }
        output.put(bits.to_ulong());
    }

    // Write tree contents to output file
    for (char c : treeValues) {
        output.put(c);
    }

    output.close();
}

// Create post order traversal, mode 0 outputs values, mode 1 outputs strucutre
void postOrderTraversal(node *root, bool mode, std::string& output) {
    if (root) {
        postOrderTraversal(root->left, mode, output);
        postOrderTraversal(root->right, mode, output);

        if (mode) {
            // Check if the current node is a leaf
            if (!root->left && !root->right) {
                output += '1'; // Node is a leaf
            } else {
                output += '0'; // Node is not a leaf
            }
        } else {
            if (!root->left && !root->right) {
                output += root->letter;
            }
        }
    }
}
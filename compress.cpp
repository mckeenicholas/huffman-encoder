#include <map>
#include <queue>
#include <bitset>

#include "compress.h"

int compress_file(std::string& inputFileName, std::string& outputFileName, bool verbose) {
    std::ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        perror("open");
        return 1;
    }

    std::map<char, node*> charMap = countCharacters(inputFile);
    inputFile.close();


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
    writeToBinary(inputFileName, outputFileName, huffmanCodes);

    // Free allocated memory for nodes
    for (auto& pair : charMap) {
        delete pair.second;
    }

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


void writeToBinary(const std::string& inputFile, const std::string& outputFile, std::map<char, std::string>& huffmanCodes) {
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

    // Write the length of the encoded data to the output file
    size_t length = encodedData.size();
    output.write(reinterpret_cast<const char*>(&length), sizeof(length));

    // Convert the binary string to actual binary data and write to the output file
    std::bitset<8> bits;
    for (size_t i = 0; i < length; i += 8) {
        for (size_t j = 0; j < 8 && i + j < length; ++j) {
            bits[j] = (encodedData[i + j] == '1');
        }
        output.put(bits.to_ulong());
    }

    input.close();
    output.close();
}

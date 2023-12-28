#ifndef EXPAND_H
#define EXPAND_H

#include "huffman_encoder.h"

int expand_file(std::string& inputFileName, std::string& outputFileName, bool verbose);

node *reconstructTree(std::string& structure, std::string& values);

#endif // EXPAND_H
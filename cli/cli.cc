#include <iostream>
#include <string.h>
#include "Chunker.hpp"

int main(int argc, char** argv) {

	int status = 0;

	if (argc == 1) {
		std::cout
			<< "No command given" << std::endl
			<< std::endl
			<< "c: compile a folder into an LZ4 Compressed Chunk" << std::endl
			<< "u: compile a folder into an uncompressed Chunk" << std::endl
			<< std::endl
			<< "example: " << argv[0] << " c inputfolder" << std::endl;
		return 1;
	}

	if (strcmp(argv[1], "c") == 0) {
		std::cout << "Compiling folder [LZ4]: " << argv[2] << std::endl;
	} else if (strcmp(argv[1], "u") == 0) {
		std::cout << "Compiling folder [RAW]: " << argv[2] << std::endl;
		status = Chunker::chunkFolder(argv[2], Chunker::None);
	} else if (strcmp(argv[1], "r") == 0) {
		std::cout << "Reading simple" << std::endl;
		Chunker::simpleRead();
	} else {
		std::cerr << "Unknown command: " << argv[1] << std::endl;
		return 1;
	}

	return status;
}

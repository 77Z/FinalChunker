#include "Chunker.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cstring>
#include "lz4.h"

void Chunker::simpleRead(const char* inputfile, const char* innerfile) {
	std::ifstream file(inputfile, std::ios::binary | std::ios::in);
	if (!file) { std::cerr << "Unable to open file" << std::endl; return; }

	// Make sure header is correct
	file.seekg(0);
	char headerVerificationBuffer[6];
	file.read(headerVerificationBuffer, 6);
	if (strcmp(headerVerificationBuffer, "SHADOW") != 0) {
		std::cerr << "Header doesn't match, this isn't a Chunker file!" << std::endl;
		return;
	}

	// Detect what kind of compression is used
	file.seekg(6);
	char compressionchar[1];
	file.read(compressionchar, 1);
	Chunker::CompressionType compression = (Chunker::CompressionType) compressionchar[0];
	// Can't print this, because it's literally 0x00
	//std::cout << "Using compression type: " << compression << std::endl;


	std::cout << LZ4_versionString() << std::endl;
}

// The header is "SHADOW"
//
// The Table of Contents Section (a.k.a the TOC Section) is stored
// right after the header and contains the offsets and file sizes of
// every file.
//
// The Data Section is stored right after the TOC Section and contains
// nothing but a blob of all the file data.
// The write cursor/offset is equal to 0 where the data section starts

int Chunker::chunkFolder(std::string folderpath, Chunker::CompressionType compression) {
	std::ifstream folderToChunk(folderpath);
	if (!folderToChunk) { std::cerr << "Unable to open given folder" << std::endl; return 1; }

	unsigned char nullchar = 0x00;

	std::fstream outfile("outputchunk", std::ios::trunc | std::ios::out | std::ios::in | std::ios::binary);

	// Write header
	const char* header = "SHADOW";
	outfile.write(header, strlen(header));

	// Write type of compression used, stored as an unsigned 8-bit number (1 byte in size)
	outfile.write((char*)&compression, sizeof(compression));

	// Write chunk filesize template
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));
	outfile.write((char*)&nullchar, sizeof(nullchar));


	uint32_t offset = 0;

	for (const auto& file : std::filesystem::recursive_directory_iterator(folderpath)) {
		std::string fileNameStr = file.path().string();
		std::cout << "Found file " << fileNameStr << std::endl;

		// Get file contents (we are not writing the actual data yet!)
		std::ifstream iteratedFile(fileNameStr);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();

		// We loaded the file to get its size

		// Write filename (Terminates at \0)
		const char* filename = fileNameStr.c_str();
		outfile.write(filename, strlen(filename));
		outfile.write((char*)&nullchar, sizeof(nullchar));

		// Write offset as an unsigned 32-bit number (4 bytes in size)
		outfile.write((char*)&offset, sizeof(offset));

		// Write filesize as an unsigned 32-bit number (4 bytes in size)
		uint32_t encodedFilesize = (uint32_t) fileData.size();
		outfile.write((char*)&encodedFilesize, sizeof(encodedFilesize));

		// Add current filesize to the offset
		offset = offset + encodedFilesize;

		// Close file for now
		iteratedFile.close();
	}


	std::cout << std::endl;

	// Overwrite the 4 bytes at 0x00000007 that stores the size
	// of the header + TOC

	std::cout << "Overwriting internal filesize data" << std::endl;

	// Seek to end to get file size
	outfile.seekg(0, std::ios::end);
	uint32_t offrsizetowrite = outfile.tellg();

	outfile.seekg(7);
	outfile.write((char*)&offrsizetowrite, sizeof(offrsizetowrite));

	// Use stage2 file handle if possible using file.seekg(0, std::ios::end)

	std::cout << std::endl;

	for (const auto& file : std::filesystem::recursive_directory_iterator(folderpath)) {
		// Second loop to actually add the data

		std::string fileNameStr = file.path().string();
		std::cout << "Adding file data to chunk: " << fileNameStr << std::endl;

		// Get file contents
		std::ifstream iteratedFile(fileNameStr);
		std::stringstream ss;
		ss << iteratedFile.rdbuf();
		std::string fileData = ss.str();

		outfile.seekg(0, std::ios::end);
		// Use << because data can contain nullchars
		outfile << fileData;


		iteratedFile.close();
	}


	outfile.close();

	folderToChunk.close();

	return 0;
}

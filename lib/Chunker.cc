#include "Chunker.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cstring>

std::string Chunker::simpleRead() { return ""; }

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

	std::fstream outfile("outputchunk", std::ios::out | std::ios::in | std::ios::binary);

	// Write header
	const char* header = "SHADOW";
	outfile.write(header, strlen(header));

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

	// Overwrite the 4 bytes at 0x00000006 that stores the size
	// of the header + TOC
	
	std::cout << "Overwriting internal filesize data" << std::endl;

	// Seek to end to get file size
	outfile.seekg(0, std::ios::end);
	uint32_t offrsizetowrite = outfile.tellg();

	outfile.seekg(6);
	outfile.write((char*)&offrsizetowrite, sizeof(offrsizetowrite));

	// Use stage2 file handle if possible using file.seekg(0, std::ios::end)
	
	/*std::ofstream outfilestage3("outputchunk", std::ios::binary);

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

		// Use << because data can contain nullchars
		outfilestage3 << fileData;


		iteratedFile.close();
	}


	outfilestage3.close();*/

	outfile.close();

	folderToChunk.close();

	return 0;
}

#pragma once

#include <string>

namespace Chunker {

	enum CompressionType : uint8_t {
		None = 0,
		LZ4 = 1,
		Snappy = 2
	};

	void simpleRead(const char* inputfile, const char* innerfile);
	int chunkFolder(std::string folderpath, CompressionType compression);

}

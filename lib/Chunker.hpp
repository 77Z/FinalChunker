#pragma once

#include <string>

namespace Chunker {

	enum CompressionType : uint8_t {
		None = 0,
		LZ4 = 1,
		Snappy = 2
	};

	inline std::string CompressionTypeToString(CompressionType compressionType) {
		switch(compressionType) {
			case None: return "None";
			case LZ4: return "LZ4";
			case Snappy: return "Snappy";
			default: return "WHAT";
		}
	}

	void listFiles(const char* inputfile);
	void simpleRead(const char* inputfile, const char* innerfile);
	int chunkFolder(std::string folderpath, CompressionType compression);

}

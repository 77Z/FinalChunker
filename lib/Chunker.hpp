#pragma once

#include <string>

namespace Chunker {

	enum CompressionType {
		None,
		LZ4
	};

	std::string simpleRead();
	int chunkFolder(std::string folderpath, CompressionType compression);

}

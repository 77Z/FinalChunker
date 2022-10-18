sources = ['Chunker.cc']

subdir('lz4')

chunklib = shared_library('chunker',
                        sources,
                        include_directories: inc,
                        dependencies: [my_lz4],
                        install: false)
staticchunklib = static_library('chunker',
                        sources,
                        include_directories: inc,
                        dependencies: [my_lz4],
                        install: false)


sources = ['Chunker.cc']

#subdir('lz4')

snappy = dependency('snappy')

chunklib = shared_library('chunker',
                        sources,
                        include_directories: inc,
                        dependencies: [snappy],
                        install: false)
staticchunklib = static_library('chunker',
                        sources,
                        include_directories: inc,
                        dependencies: [snappy],
                        install: false)


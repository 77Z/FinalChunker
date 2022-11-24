sources = ['Chunker.cc']

#subdir('lz4')

my_lz4 = dependency('liblz4')

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


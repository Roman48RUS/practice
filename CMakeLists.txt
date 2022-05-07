cmake_minimum_required(VERSION 3.1)


project(Codec)

#add_library(Codec SHARED "../src/a51_1.c")
add_executable(Codec.A51 "../src/a51_1.c")
add_executable(Codec.encoder "../src/encoder_example.c")

target_link_libraries(Codec.A51)
target_link_libraries(Codec.encoder ogg vorbis vorbisenc)

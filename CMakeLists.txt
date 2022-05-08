cmake_minimum_required(VERSION 3.1)


project(Codec)

#add_library(Codec SHARED "../src/a51_1.c")
add_library(Codec.A51 SHARED "../src/a51.c")
add_library(Codec.encoder SHARED "../src/encoder.c")
add_library(Codec.decoder SHARED "../src/decoder.c")
add_executable(Codec.main "../src/main.c")

target_link_libraries(Codec.main Codec.A51 Codec.encoder Codec.decoder ogg vorbis vorbisenc)

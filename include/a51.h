typedef unsigned char byte;
typedef unsigned long word;

byte* encode(const byte* data, int size, const byte* key, const word startFrame, byte* output);
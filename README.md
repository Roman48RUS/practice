# practice
first year MIREA practice   
Simple audio file codec with elements of encryption using A51/1 algorithm and OGG codec    


REQUIRES OGG AND VORBIS LIBRARIES!    
https://github.com/xiph/ogg    
https://github.com/xiph/vorbis    

## Build instructions:    

INSTALL OGG AND VORBIS LIBRARIES!    
Download Codec    
inside project folder:    
mkdir bin    
cd bin    
cmake ..    
make    

## Usage:    
In bin folder run ./Codec.main with options:    
First argument:    
-cf encoding and decoding regular files (for example text or audio WITHOUT compression)    
-ca compress and encrypt audio file    
-da decompress and decrypt audio file    
    
Second argument: filename for encryption    
Third argument: -o to specify the output file (default: output)    
Fourth argument: output file name    

## Examples    
./Codec.main -cf text.txt -- encryption (decryption) of a text file to default output file    
./Codec.main -cf text.txt -o cipher -- ecryption of a text file to "cipher" output file    
./Codec.main -ca audio.wav -o cipher -- compression and encryption of an audio file to "cipher" output file    
./Codec.main -da cipher -o decoded.wav -- decryption and decompression of an encrypted audio file to "decoded.wav" output file    
    
    
    
    
### Key and initial frame number cannot be specified yet!    
coding: Roman48RUS and stepavv    
documents: stepavv and Roman48RUS    

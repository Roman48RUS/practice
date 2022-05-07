#include <stdio.h>
#include <stdbool.h>
 
/* Маски для сдвиговых регистров */
#define R1MASK  0x07FFFF // 19 бит, 0..18
#define R2MASK  0x3FFFFF // 22 бита, 0..21
#define R3MASK  0x7FFFFF // 23 бита, 0..22
 
/* Средние биты регистров */
#define R1MID   0x000100 /* бит 8 */
#define R2MID   0x000400 /* бит 10 */
#define R3MID   0x000400 /* бит 10 */
 
/* Биты обратных связей
 * x^19 + x^5 + x^2 + x + 1, x^22 + x + 1,
 * x^23 + x^15 + x^2 + x + 1. */
#define R1TAPS  0x072000 /* биты 18,17,16,13 */
#define R2TAPS  0x300000 /* биты 21,20 */
#define R3TAPS  0x700080 /* биты 22,21,20,7 */
 
/* Выходные биты */
#define R1OUT   0x040000 /* бит 18 */
#define R2OUT   0x200000 /* бит 21 */
#define R3OUT   0x400000 /* бит 22 */
 
typedef unsigned char byte;
typedef unsigned long word;
typedef word bit;
 
/* Четность 32-х битного слова*/
bit parity(word x) {
    x ^= x>>16;
    x ^= x>>8;
    x ^= x>>4;
    x ^= x>>2;
    x ^= x>>1;
    return x&1;
}
 
/* Такт одного регистра */
word clockone(word reg, word mask, word taps) {
    word t = reg & taps;
    reg = (reg << 1) & mask;
    reg |= parity(t);
    return reg;
}
 
/* Три сдвиговых регистра */
word R1, R2, R3;
 
/* Мажоритарная функция 3 средних битов */
bit majority() {
    int sum;
    sum = parity(R1&R1MID) + parity(R2&R2MID) + parity(R3&R3MID);
    if (sum >= 2)
        return 1;
    else
        return 0;
}
 
/* Тактирование двух или трех регистров*/
void clock() {
    bit maj = majority();
    if (((R1&R1MID)!=0) == maj)
        R1 = clockone(R1, R1MASK, R1TAPS);
    if (((R2&R2MID)!=0) == maj)
        R2 = clockone(R2, R2MASK, R2TAPS);
    if (((R3&R3MID)!=0) == maj)
        R3 = clockone(R3, R3MASK, R3TAPS);
}
 
/* Тактирование трех регистров */
void clockallthree() {
    R1 = clockone(R1, R1MASK, R1TAPS);
    R2 = clockone(R2, R2MASK, R2TAPS);
    R3 = clockone(R3, R3MASK, R3TAPS);
}
 
/* Генерирование выходного бита */
bit getbit() {
    return parity(R1&R1OUT)^parity(R2&R2OUT)^parity(R3&R3OUT);
}
 
/* Инициализация ключа */
void keysetup(const byte key[8], word frame) {
    int i;
    bit keybit, framebit;
 
    /* Зануление регистров. */
    R1 = R2 = R3 = 0;
 
    /* Загрузка ключа в регистры */
    for (i=0; i<64; i++) {
        clockallthree();
        keybit = (key[i/8] >> (i&7)) & 1;
        R1 ^= keybit; R2 ^= keybit; R3 ^= keybit;
    }
 
    /* Загрузка номера фрейма в регистры */
    for (i=0; i<22; i++) {
        clockallthree(); 
        framebit = (frame >> i) & 1;
        R1 ^= framebit; R2 ^= framebit; R3 ^= framebit;
    }
 
    /* 100 тактов сдвиговых регистров */
    for (i=0; i<100; i++) {
        clock();
    }
 
    /* Now the key is properly set up. */
}
    
/* Генерация 228 битов вывода, по 114 бит для передачи в разные стороны */
void run(byte AtoBkeystream[], byte BtoAkeystream[]) {
    int i;
 
    /* Зануление выходных буферов. */
    for (i=0; i<=113; i++)
        AtoBkeystream[i/8] = BtoAkeystream[i/8] = 0;
    
    /* Генерация 114 бит в сторону А-Б */
    for (i=0; i<114; i++) {
        clock();
        AtoBkeystream[i/8] |= getbit() << (7-(i&7));
    }
 
    /* Генерация 114 бит в сторону Б-А */
    for (i=0; i<114; i++) {
        clock();
        BtoAkeystream[i/8] |= getbit() << (7-(i&7));
    }
}


byte* encode(const byte* data, int size, const byte* key, const word startFrame, byte* output)
{
    word frame = startFrame;

    byte AtoB[114], BtoA[114];
    int i = 0;

    bool bits[size * 8];
    bool encBits[size * 8];

    for (i=0; i<size; i++)
    {
        for (int j=0; j<8; j++)
        {
            bits[i * 8 + j] = data[i] & (1 << 7) >> j; //Big endian
        }
    }

    unsigned dataFrames = size*8/114;
    unsigned lastFrameLength = size*8 % 114;
    bool AtoBBits[114];


    for (i=0; i<dataFrames; i++)
    {
        keysetup(key, frame);
        run(AtoB, BtoA);
        for (int j=0; j<114; j++)
        {
        AtoBBits[j] = AtoB[j / 8] & ((1 << 7) >> (j % 8)); //Big endian MSB first
        }

        for (int j=0; j<114; j++)
        {
            encBits[i * 114 + j] = bits[i * 114 + j] ^ AtoBBits[j];
        }
        frame++;
    }
    keysetup(key, frame);
    run(AtoB, BtoA);
    for (int j=0; j<114; j++)
    {
        AtoBBits[j] = AtoB[j / 8] & ((1 << 7) >> (j % 8)); //Big endian MSB first
    }
    for (int j=0; j<lastFrameLength; j++)
    {
        encBits[dataFrames * 114 + j] = bits[dataFrames * 114 + j] ^ AtoBBits[j];
    }
    
    for (i=0; i<size; i++)
    {
        byte buffer = 0;
        for (int j=0; j<8; j++)
            if (encBits[i * 8 + j])
                buffer |= ((1 << 7) >> j);
        output[i] = buffer;
        
    }

}




 
int main(void) {
    byte key[8] = {0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    word frame = 0x001;
    FILE* in = fopen("test.ogg", "rb");
    FILE* out = fopen("cipher", "wb");
    FILE* decipher = fopen("decipher.ogg", "wb");
    fseek(in, 0, SEEK_END);
    int size = ftell(in);
    rewind(in);
    byte data[size];
    byte encodedData[size];
    byte decodedData[size];
    fread(data, 1, size, in);

    encode(data, size, key, frame, encodedData);
    fwrite(encodedData, 1, size, out);
    fwrite(encodedData, 1, size, out);

    printf("\n");
    encode(encodedData, size, key, frame, decodedData);
    fwrite(decodedData, 1, size, decipher);
    printf("\n");

    return 0;
}
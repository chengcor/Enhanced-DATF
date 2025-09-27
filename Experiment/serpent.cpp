#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <bitset>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include <string>
#include <omp.h> 
using namespace std;

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct {
    u32 x[4];
} Serpent_state_t;

thread_local std::mt19937_64 rng(std::random_device{}());

#define rotateLeft(s, count) ((s << count) | (s >> (32 - count))) & 0xffffffff
#define SBOX0(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r3 ^= r0; r4 =  r1; \
    r1 &= r3; r4 ^= r2; \
    r1 ^= r0; r0 |= r3; \
    r0 ^= r4; r4 ^= r3; \
    r3 ^= r2; r2 |= r1; \
    r2 ^= r4; r4 = ~r4; \
    r4 |= r1; r1 ^= r3; \
    r1 ^= r4; r3 |= r0; \
    r1 ^= r3; r4 ^= r3; \
    \
    w = r1; x = r4; y = r2; z = r0; \
  }
#define SBOX1(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r0 = ~r0; r2 = ~r2; \
    r4 =  r0; r0 &= r1; \
    r2 ^= r0; r0 |= r3; \
    r3 ^= r2; r1 ^= r0; \
    r0 ^= r4; r4 |= r1; \
    r1 ^= r3; r2 |= r0; \
    r2 &= r4; r0 ^= r1; \
    r1 &= r2; \
    r1 ^= r0; r0 &= r2; \
    r0 ^= r4; \
    \
    w = r2; x = r0; y = r3; z = r1; \
  }
#define SBOX2(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r4 =  r0; r0 &= r2; \
    r0 ^= r3; r2 ^= r1; \
    r2 ^= r0; r3 |= r4; \
    r3 ^= r1; r4 ^= r2; \
    r1 =  r3; r3 |= r4; \
    r3 ^= r0; r0 &= r1; \
    r4 ^= r0; r1 ^= r3; \
    r1 ^= r4; r4 = ~r4; \
    \
    w = r2; x = r3; y = r1; z = r4; \
  }
#define SBOX3(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r4 =  r0; r0 |= r3; \
    r3 ^= r1; r1 &= r4; \
    r4 ^= r2; r2 ^= r3; \
    r3 &= r0; r4 |= r1; \
    r3 ^= r4; r0 ^= r1; \
    r4 &= r0; r1 ^= r3; \
    r4 ^= r2; r1 |= r0; \
    r1 ^= r2; r0 ^= r3; \
    r2 =  r1; r1 |= r3; \
    r1 ^= r0; \
    \
    w = r1; x = r2; y = r3; z = r4; \
  }
#define SBOX4(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r1 ^= r3; r3 = ~r3; \
    r2 ^= r3; r3 ^= r0; \
    r4 =  r1; r1 &= r3; \
    r1 ^= r2; r4 ^= r3; \
    r0 ^= r4; r2 &= r4; \
    r2 ^= r0; r0 &= r1; \
    r3 ^= r0; r4 |= r1; \
    r4 ^= r0; r0 |= r3; \
    r0 ^= r2; r2 &= r3; \
    r0 = ~r0; r4 ^= r2; \
    \
    w = r1; x = r4; y = r0; z = r3; \
  }
#define SBOX5(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r0 ^= r1; r1 ^= r3; \
    r3 = ~r3; r4 =  r1; \
    r1 &= r0; r2 ^= r3; \
    r1 ^= r2; r2 |= r4; \
    r4 ^= r3; r3 &= r1; \
    r3 ^= r0; r4 ^= r1; \
    r4 ^= r2; r2 ^= r0; \
    r0 &= r3; r2 = ~r2; \
    r0 ^= r4; r4 |= r3; \
    r2 ^= r4; \
    \
    w = r1; x = r3; y = r0; z = r2; \
  }
#define SBOX6(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r2 = ~r2; r4 =  r3; \
    r3 &= r0; r0 ^= r4; \
    r3 ^= r2; r2 |= r4; \
    r1 ^= r3; r2 ^= r0; \
    r0 |= r1; r2 ^= r1; \
    r4 ^= r0; r0 |= r3; \
    r0 ^= r2; r4 ^= r3; \
    r4 ^= r0; r3 = ~r3; \
    r2 &= r4; \
    r2 ^= r3; \
    \
    w = r0; x = r1; y = r4; z = r2; \
  }
#define SBOX7(r0, r1, r2, r3, w, x, y, z) \
  { \
    u32 r4; \
    \
    r4 =  r1; r1 |= r2; \
    r1 ^= r3; r4 ^= r2; \
    r2 ^= r1; r3 |= r4; \
    r3 &= r0; r4 ^= r2; \
    r3 ^= r1; r1 |= r4; \
    r1 ^= r0; r0 |= r4; \
    r0 ^= r2; r1 ^= r4; \
    r2 ^= r1; r1 &= r0; \
    r1 ^= r4; r2 = ~r2; \
    r2 |= r0; \
    r4 ^= r2; \
    \
    w = r4; x = r3; y = r1; z = r0; \
  }

#define SBOX_CASE(which, a, b)                         \
  switch(which) {                                                \
    case 0: SBOX0(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 1: SBOX1(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 2: SBOX2(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 3: SBOX3(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 4: SBOX4(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 5: SBOX5(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 6: SBOX6(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
    case 7: SBOX7(a.x[0], a.x[1], a.x[2], a.x[3],  \
               b.x[0], b.x[1], b.x[2], b.x[3]); break; \
  }

#define LT(a)                  \
  {                                                   \
    a.x[0] = rotateLeft (a.x[0], 13);                    \
    a.x[2] = rotateLeft (a.x[2], 3);                     \
    a.x[1] = a.x[1] ^ a.x[0] ^ a.x[2];        \
    a.x[3] = a.x[3] ^ a.x[2] ^ (a.x[0] << 3); \
    a.x[1] = rotateLeft (a.x[1], 1);                     \
    a.x[3] = rotateLeft (a.x[3], 7);                     \
    a.x[0] = a.x[0] ^ a.x[1] ^ a.x[3];        \
    a.x[2] = a.x[2] ^ a.x[3] ^ (a.x[1] << 7); \
    a.x[0] = rotateLeft (a.x[0], 5);                     \
    a.x[2] = rotateLeft (a.x[2], 22);                    \
  }

#define BLOCK_COPY(block_dst, block_src) \
  {                                      \
    block_dst.x[0] = block_src.x[0];         \
    block_dst.x[1] = block_src.x[1];         \
    block_dst.x[2] = block_src.x[2];         \
    block_dst.x[3] = block_src.x[3];         \
  }

u32 Serpent_u32(int r, int offset, u64 loc, bitset<128> IV, bitset<128> lamb, u32 Key[16])
{
    int i;
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    Serpent_state_t s1, s2;
    for (i = 0; i < 4; i++)
        s1.x[i] = dist(rng);

    u32 t;
    for (int j = 0; j < 4; j++)
    {
        t = 0;
        for (i = 0; i < 32; i++)
            if (IV[i + 32 * j])
                t ^= 1 << (31 - i);
        s2.x[j] = s1.x[j] ^ t;
    }

    Serpent_state_t temp;
    for (int loc = 0; loc < r; loc++)
    {
        if (loc != 0)
        {
            for (i = 0; i < 4; i++)
                s1.x[i] ^= Key[4 * loc + i];
        }
        SBOX_CASE((offset + loc) % 8, s1, temp);
        // if (loc<r-1)
        LT(temp);

        BLOCK_COPY(s1, temp);
    }

    for (int loc = 0; loc < r; loc++)
    {
        if (loc != 0)
        {
            for (i = 0; i < 4; i++)
                s2.x[i] ^= Key[4 * loc + i];
        }
        SBOX_CASE((offset + loc) % 8, s2, temp);
        //if (loc < r - 1)
        LT(temp);
        BLOCK_COPY(s2, temp);
    }
    u32 result = 0;
    for (i = 0; i < 32; i++)
    {
        if (lamb[i])  result ^= (s1.x[0] >> (31 - i) & 1) ^ (s2.x[0] >> (31 - i) & 1);
        if (lamb[i + 32])  result ^= (s1.x[1] >> (31 - i) & 1) ^ (s2.x[1] >> (31 - i) & 1);
        if (lamb[i + 64])  result ^= (s1.x[2] >> (31 - i) & 1) ^ (s2.x[2] >> (31 - i) & 1);
        if (lamb[i + 96])  result ^= (s1.x[3] >> (31 - i) & 1) ^ (s2.x[3] >> (31 - i) & 1);
    }
    return result;

}




int main()
{

    const u64 count = 4194304;

    int r = 2;
    int offset = 3;
    vector<int> delta = { 15, 21, 58, 93, 94, 116 };
    vector<int> lambda = { 63,66 };


   // int r = 3;
   // int offset = 2;
  //  vector<int> delta = { 78 };
   // vector<int> lambda = { 61,64 };

    cout << "r:  " << r << endl;
    cout << "offset:  " << offset << endl;
    cout << "delta:  ";
    for (int num : delta)
    {
        cout << num << ", ";
    }
    cout << endl;

    cout << "lambda:  ";
    for (int num : lambda)
    {
        cout << num << ", ";
    }
    cout << endl;


    bitset<128> IV(0);
    for (int num : delta)
        IV.set(num);

    bitset<128> lamb(0);
    for (int num : lambda)
        lamb.set(num);


    double b = 0;

#pragma omp parallel for
    for (int loc = 1; loc <= 64; loc++)
    {

        uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

        u32 Key[16];
        for (int k = 0; k < 16; k++)
            Key[k] = dist(rng);

        u64 sum = 0;

        for (u64 i = 0; i < count; i++)
            sum += Serpent_u32(r, offset, i, IV, lamb, Key);

#pragma omp critical
        {
            b += 0.5 - (1.0 * sum / count);
        }
    }

    cout << "Average bias : " << b / 64 << endl;

    system("pause");
    return 0;
}


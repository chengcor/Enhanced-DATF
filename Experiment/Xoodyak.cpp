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
thread_local std::mt19937_64 rng(std::random_device{}());

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

struct XoodooState {
    uint32_t state[4][3] = { {0} }; // [row][plane]
};

inline uint32_t ROTL32(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}


void theta(XoodooState& s)
{
    uint32_t P[4], E[4];
    for (int i = 0; i < 4; i++)
        P[i] = s.state[i][0] ^ s.state[i][1] ^ s.state[i][2];

    for (int i = 0; i < 4; i++)
        E[i] = ROTL32(P[(i + 3) % 4], 5) ^ ROTL32(P[(i + 3) % 4], 14);

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            s.state[i][j] ^= E[i];
}


void rho_west(XoodooState& s)
{
    XoodooState t = s;
    for (int row = 0; row < 4; row++)
    {
        s.state[row][1] = t.state[(row + 3) % 4][1];
        s.state[row][2] = ROTL32(t.state[row][2], 11);
    }
}

void iota(XoodooState& s, int C)
{
    s.state[0][0] ^= C;
}

void chi(XoodooState& s)
{
    for (int row = 0; row < 4; row++)
    {
        uint32_t a = s.state[row][0];
        uint32_t b = s.state[row][1];
        uint32_t c = s.state[row][2];
        s.state[row][0] = a ^ (~b & c);
        s.state[row][1] = b ^ (~c & a);
        s.state[row][2] = c ^ (~a & b);
    }
}


void rho_east(XoodooState& s)
{
    XoodooState t = s;

    for (int row = 0; row < 4; row++)
    {
        s.state[row][1] = ROTL32(t.state[row][1], 1);
        s.state[row][2] = ROTL32(t.state[(row + 2) % 4][2], 8);
    }
}





void RF(XoodooState& s, unsigned int C)
{
    theta(s);
    rho_west(s);
    iota(s, C);
    chi(s);
    rho_east(s);
}


u8 Xoodyak(int r, bitset<384> IV, bitset<384> lamb, u32 key[4])
{
    unsigned int C[12] = { 0x58, 0x38, 0x3C0, 0xD0, 0x120, 0x14, 0x60, 0x2C, 0x380, 0xF0, 0x1A0, 0x12 };

    XoodooState s1, s2;
    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    for (int i = 0; i < 4; i++)
    {
        s1.state[i][0] = key[i];
        s1.state[i][1] = dist(rng);
        s1.state[i][2] = dist(rng);

    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            s2.state[i][j] = s1.state[i][j];

    for (int i = 0; i < 384; i++)
        if (IV[i])
            s2.state[(i % 128) / 32][i / 128] ^= 1 << (i % 32);


    //chi(s1);
    //rho_east(s1);

   // chi(s2);
   // rho_east(s2);
    for (int round = 0; round < r; round++)
    {
        RF(s1, C[round]);
        RF(s2, C[round]);
    }

    u8 result = 0;
    for (int i = 0; i < 384; i++)
        if (lamb[i])
            result ^= (s1.state[(i % 128) / 32][i / 128] >> (i % 32) & 1) ^ (s2.state[(i % 128) / 32][i / 128] >> (i % 32) & 1);

    return result;

}





int main()
{

    const u64 count = 1024 * 1024;

    //int r = 4;         // -0.0244141
   // vector<int> delta = { 0,256 };
   // vector<int> lambda = { 0 };

    int r = 4;       // 0.0012207
    vector<int> delta = { 128,256 };
    vector<int> lambda = { 128+15 };
 
 
   // int r = 2;       //0.5
   // vector<int> delta = { 0,2,4,5,8,9,11,13,14,15,16,17,18,19,22,25,26,27,28,31,36,37,49,52,53,56,57,60,61,65,67,70,71,73,75,82,83,86,89,90,93,94,95,96,97,98,99,102,103,109,110,111,112,113,116,117,120,121,123,126,127,128,130,131,132,133,136,137,139,141,142,143,144,145,146,147,150,153,154,155,156,159,165,177,180,181,184,185,188,189,193,195,198,199,201,203,210,211,214,217,218,221,222,223,224,225,226,227,230,231,237,238,239,240,241,244,245,248,249,251,254,255,256,258,260,261,264,265,267,269,270,271,272,273,274,275,278,281,282,283,284,287,293,305,308,309,312,313,316,317,321,323,326,327,329,331,338,339,342,345,346,349,350,351,352,353,354,355,358,359,365,366,367,368,369,372,373,376,377,379,382,383 };

   // vector<int> lambda = { 10,171 };



    bitset<384> IV(0);
    for (int num : delta)
        IV.set(num);

    bitset<384> lamb(0);
    for (int num : lambda)
        lamb.set(num);

    double b = 0;

#pragma omp parallel for
    for (int loc = 1; loc <= 32; loc++)
    {
        uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
        u32 key[4];
        for (int k = 0; k < 4; k++)
            key[k] = dist(rng);

        u64 sum = 0;
        for (u64 i = 0; i < count; i++)
            sum += Xoodyak(r, IV, lamb, key);


#pragma omp critical
        {
            b += 0.5 - (1.0 * sum / count);            
        }
       

    }
    
    cout << "Average bias : " << b / 32 << endl;
    system("pause");
    return 0;


}


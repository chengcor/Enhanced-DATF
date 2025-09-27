#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <bitset>
#include <iomanip>
#include <random>
#include <omp.h> 
using namespace std;



typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
    u64 x[5];
} ascon_state_t;

#define ASCON_128_IV (((u64)(16 * 8) << 56) | ((u64)(8 * 8) << 48) | ((u64)(12) << 40) | ((u64)(6) << 32))

#define k3 (K[0] >> 4 & 1)
#define k89 (K[11] >> 6 & 1)
#define v28 (nonce[3] >> 3 & 1)
#define v53 (nonce[6] >> 2 & 1)
#define v31 (nonce[3] >> 0 & 1)
#define v87 (nonce[10] >> 0 & 1)
#define v76 (nonce[9] >> 3 & 1)
#define v22 (nonce[2] >> 1 & 1)
#define k31 (K[3] >> 0 & 1)
#define v25 (nonce[3] >> 6 & 1)
#define k11 (K[1] >> 4 & 1)
#define k19 (K[2] >> 4 & 1)
#define k73 (K[9] >> 6 & 1)
#define k86 (K[10] >> 1 & 1)
#define k22 (K[2] >> 1 & 1)
#define v111 (nonce[13] >> 0 & 1)
#define v3 (nonce[0] >> 4 & 1)
#define v67 (nonce[8] >> 4 & 1)
#define v85 (nonce[10] >> 2 & 1)
#define v121 (nonce[15] >> 6 & 1)
#define v12 (nonce[1] >> 3 & 1)
#define v95 (nonce[11] >> 0 & 1)
#define k51 (K[6] >> 4 & 1)
#define k92 (K[11] >> 3 & 1)
#define v47 (nonce[5] >> 0 & 1)
#define k111 (K[13] >> 0 & 1)
#define k44 (K[5] >> 3 & 1)
#define k9 (K[1] >> 6 & 1)
#define v57 (nonce[7] >> 6 & 1)
#define k67 (K[8] >> 4 & 1)
#define v82 (nonce[10] >> 5 & 1)
#define k12 (K[1] >> 3 & 1)
#define v44 (nonce[5] >> 3 & 1)
#define v75 (nonce[9] >> 4 & 1)
#define k57 (K[7] >> 6 & 1)
#define v19 (nonce[2] >> 4 & 1)
#define k42 (K[5] >> 5 & 1)
#define v51 (nonce[6] >> 4 & 1)
#define k21 (K[2] >> 2 & 1)
#define k75 (K[9] >> 4 & 1)
#define k47 (K[5] >> 0 & 1)
#define v66 (nonce[8] >> 5 & 1)
#define v106 (nonce[13] >> 5 & 1)
#define k25 (K[3] >> 6 & 1)
#define k66 (K[8] >> 5 & 1)
#define v83 (nonce[10] >> 4 & 1)
#define k53 (K[6] >> 2 & 1)
#define k118 (K[14] >> 1 & 1)
#define k28 (K[3] >> 3 & 1)
#define v11 (nonce[1] >> 4 & 1)
#define k2 (K[0] >> 5 & 1)
#define v21 (nonce[2] >> 2 & 1)
#define k23 (K[2] >> 0 & 1)
#define k54 (K[6] >> 1 & 1)
#define v54 (nonce[6] >> 1 & 1)
#define k83 (K[10] >> 4 & 1)
#define k108 (K[13] >> 3 & 1)
#define v86 (nonce[10] >> 1 & 1)
#define k117 (K[14] >> 2 & 1)
#define v42 (nonce[5] >> 5 & 1)
#define k82 (K[10] >> 5 & 1)
#define v23 (nonce[2] >> 0 & 1)
#define v92 (nonce[11] >> 3 & 1)
#define k18 (K[2] >> 5 & 1)
#define v115 (nonce[14] >> 4 & 1)
#define k95 (K[11] >> 0 & 1)

/* set byte in 64-bit Ascon word */
#define SETBYTE(b, i) ((u64)(b) << (56 - 8 * (i)))


constexpr u8 C[7] = { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4,0xa5,0x96 };

thread_local std::mt19937_64 rng(std::random_device{}());


u64 ROR(u64 x, int n)
{
    return x >> n | x << (-n & 63);
}

/* load bytes into 64-bit Ascon word */
u64 LOADBYTES(u8* bytes, int n)
{
    int i;
    u64 x = 0;
    for (i = 0; i < n; i++)
        x |= SETBYTE(bytes[i], i);
    return x;
}


ascon_state_t Ascon(int r, u8* K)
{

    int i;

    std::uniform_int_distribution<unsigned short> dist_256(0, 255);  // 0-255
    std::uniform_int_distribution<unsigned short> dist_128(0, 127);  // 0-127

    u8 nonce[16];
    for (int i = 1; i < 16; i++)
        nonce[i] = static_cast<u8>(dist_256(rng));

    nonce[0] = static_cast<u8>(dist_128(rng));
    nonce[8] = static_cast<u8>(dist_128(rng));


    
    u8 v89 = k3 & k67 ^ k3 & v3 ^ k3 ^ k25 & k89 ^ k25 & v25 ^ k25 ^ k67 & v3 ^ k67 ^ k89 & v25 ^ k89 ^ v3 ^ v25 ^ v67 + 1;
    u8 v118 = k23 & v87 ^ k23 ^ k47 ^ k54 ^ k57 & v121 ^ k57 ^ k111 ^ k118 ^ v23 ^ v47 ^ v54 ^ v57 ^ v87 ^ v111 ^ v121;
    u8 v2 = k2 ^ k9 ^ k12 & v76 ^ k12 ^ k19 & v83 ^ k42 & v106 ^ k42 ^ k66 ^ k73 ^ k83 ^ v12 ^ v42 ^ v66 ^ v76 ^ v106;
    u8 v108 = k19 & k83 ^ k19 & v19 ^ k19 ^ k22 & k86 ^ k22 & v22 ^ k22 ^ k44 & k108 ^ k44 & v44 ^ k44 ^ k83 & v19 ^ k83 ^ k86 & v22 ^ k86 ^ k108 & v44 ^ k108 ^ v19 ^ v22 ^ v44 ^ v83 ^ v86;
    u8 v117 = k28 & k92 ^ k28 & v28 ^ k28 ^ k31 & k95 ^ k31 & v31 ^ k31 ^ k53 & k117 ^ k53 & v53 ^ k53 ^ k92 & v28 ^ k92 ^ k95 & v31 ^ k95 ^ k117 & v53 ^ k117 ^ v28 ^ v31 ^ v53 ^ v92 ^ v95;
    u8 v18 = k11 ^ k18 ^ k21 & v85 ^ k28 & v92 ^ k51 & v115 ^ k51 ^ k75 ^ k82 ^ k92 ^ v11 ^ v21 ^ v51 ^ v75 ^ v82 ^ v85 ^ v115 + 1;

    nonce[0] = (nonce[0] & 0xdf) ^ (v2 << 5);
    nonce[11] = (nonce[11] & 0xbf) ^ (v89 << 6);
    nonce[13] = (nonce[13] & 0xf7) ^ (v108 << 3);
    nonce[14] = (nonce[14] & 0xfb) ^ (v117 << 2);
    nonce[14] = (nonce[14] & 0xfd) ^ (v118 << 1);
    nonce[2] = (nonce[2] & 0xdf) ^ (v18 << 5);
    

    u64 K0 = LOADBYTES(K, 8);
    u64 K1 = LOADBYTES(K + 8, 8);
    u64 N0 = LOADBYTES(nonce, 8);
    u64 N1 = LOADBYTES(nonce + 8, 8);




    /* initialize */
    ascon_state_t s1, s2;
    s1.x[0] = ASCON_128_IV;
    s1.x[1] = K0;
    s1.x[2] = K1;
    s1.x[3] = N0;
    s1.x[4] = N1;





    for (i = 0; i < r; i++)
    {
        ascon_state_t t;
        /* addition of round constant */
        s1.x[2] ^= C[i];
        /* printstate(" round constant", s); */
        /* substitution layer */
        s1.x[0] ^= s1.x[4];
        s1.x[4] ^= s1.x[3];
        s1.x[2] ^= s1.x[1];

        /* start of keccak s-box */
        t.x[0] = s1.x[0] ^ (~s1.x[1] & s1.x[2]);
        t.x[1] = s1.x[1] ^ (~s1.x[2] & s1.x[3]);
        t.x[2] = s1.x[2] ^ (~s1.x[3] & s1.x[4]);
        t.x[3] = s1.x[3] ^ (~s1.x[4] & s1.x[0]);
        t.x[4] = s1.x[4] ^ (~s1.x[0] & s1.x[1]);
        /* end of keccak s-box */
        t.x[1] ^= t.x[0];
        t.x[0] ^= t.x[4];
        t.x[3] ^= t.x[2];
        t.x[2] = ~t.x[2];

        if (i < r - 1)
        {
            /* linear diffusion layer */
            s1.x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);
            s1.x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);
            s1.x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);
            s1.x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);
            s1.x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);
        }
        else
        {
            s1.x[0] = t.x[0]; s1.x[1] = t.x[1]; s1.x[2] = t.x[2]; s1.x[3] = t.x[3]; s1.x[4] = t.x[4];
        }
    }



    nonce[0] ^= 1 << 7;
    nonce[8] ^= 1 << 7;

    N0 = LOADBYTES(nonce, 8);
    N1 = LOADBYTES(nonce + 8, 8);


    s2.x[0] = ASCON_128_IV;
    s2.x[1] = K0;
    s2.x[2] = K1;
    s2.x[3] = N0;
    s2.x[4] = N1;

    for (i = 0; i < r; i++)
    {
        ascon_state_t t;
        /* addition of round constant */
        s2.x[2] ^= C[i];
        /* printstate(" round constant", s); */
        /* substitution layer */
        s2.x[0] ^= s2.x[4];
        s2.x[4] ^= s2.x[3];
        s2.x[2] ^= s2.x[1];
        /* start of keccak s-box */
        t.x[0] = s2.x[0] ^ (~s2.x[1] & s2.x[2]);
        t.x[1] = s2.x[1] ^ (~s2.x[2] & s2.x[3]);
        t.x[2] = s2.x[2] ^ (~s2.x[3] & s2.x[4]);
        t.x[3] = s2.x[3] ^ (~s2.x[4] & s2.x[0]);
        t.x[4] = s2.x[4] ^ (~s2.x[0] & s2.x[1]);
        /* end of keccak s-box */
        t.x[1] ^= t.x[0];
        t.x[0] ^= t.x[4];
        t.x[3] ^= t.x[2];
        t.x[2] = ~t.x[2];

        if (i < r - 1)
        {
            /* linear diffusion layer */
            s2.x[0] = t.x[0] ^ ROR(t.x[0], 19) ^ ROR(t.x[0], 28);
            s2.x[1] = t.x[1] ^ ROR(t.x[1], 61) ^ ROR(t.x[1], 39);
            s2.x[2] = t.x[2] ^ ROR(t.x[2], 1) ^ ROR(t.x[2], 6);
            s2.x[3] = t.x[3] ^ ROR(t.x[3], 10) ^ ROR(t.x[3], 17);
            s2.x[4] = t.x[4] ^ ROR(t.x[4], 7) ^ ROR(t.x[4], 41);
        }
        else
        {
            s2.x[0] = t.x[0]; s2.x[1] = t.x[1]; s2.x[2] = t.x[2]; s2.x[3] = t.x[3]; s2.x[4] = t.x[4];
        }

    }

    ascon_state_t temp;
    for (i = 0; i < 5; i++)
        temp.x[i] = s1.x[i] ^ s2.x[i];

    return temp;

}




int main()
{
    int r = 5;
    const u32 count = 1048576;
    const u32 num = 64;


    cout << "Calculate the bias of the CDL distinguisher for 5-round Ascon, excluding linear layers." << endl;

    double* b1 = new double[320]();


#pragma omp parallel for
    for (int loc = 1; loc <= num; loc++)
    {

        uniform_int_distribution<unsigned short> dist_256(0, 255);
        uniform_int_distribution<unsigned short> dist_128(0, 127);

        u8 K[16];
        for (int k = 1; k < 16; k++)
            K[k] = static_cast<u8>(dist_256(rng));
        K[0] = static_cast<u8>(dist_128(rng));
        K[8] = static_cast<u8>(dist_128(rng));


        int* sum1 = new int[320]();


        bitset<320> t;
        for (u64 i = 0; i < count; i++)
        {
            ascon_state_t det = Ascon(r, K);
            for (int k = 0; k < 5; k++)
                for (int j = 0; j < 64; j++)
                    t[k * 64 + j] = (det.x[k] >> (63 - j)) & 1;

            for (int k = 0; k < 320; k++)
                sum1[k] += t[k];

        }


#pragma omp critical
        {
            for (int j = 0; j < 320; j++)
                b1[j] += 0.5 - (1.0 * sum1[j] / count);
        }

        delete[] sum1;
    }


    for (int j = 0; j < 64; j++)
    {
        if (fabs(b1[j] / num)>0.001)
        {
            cout << " loc: " << j << "     bias: " << b1[j] / num << endl;
        }
    }
    delete[] b1;

    system("pause");
    return 0;

}

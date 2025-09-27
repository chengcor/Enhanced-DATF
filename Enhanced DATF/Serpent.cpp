#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <map>
#include <mutex>
#include <time.h>
#include <sstream>
#include <thread>
#include <string>
using namespace std;

#define round 3
const int VARIABLE_COUNT = 128 + 2 + 2 * 128 * round;      // x + const 1 + ATF
const int loc_x = 128;
const int loc_1 = 128 + 1;
using PolyTerm = bitset<VARIABLE_COUNT>;
using Poly = vector<PolyTerm>;

typedef struct {
    unsigned int x[4];
} Serpent_state_t;



#define rL(s, count) ((s << count) | (s >> (32 - count))) & 0xffffffff
#define LT2(a)                  \
  {                                                   \
    a.x[0] = rL(a.x[0], 13);                    \
    a.x[2] = rL(a.x[2], 3);                     \
    a.x[1] = a.x[1] ^ a.x[0] ^ a.x[2];           \
    a.x[3] = a.x[3] ^ a.x[2] ^ (a.x[0] << 3);    \
    a.x[1] = rL(a.x[1], 1);                     \
    a.x[3] = rL(a.x[3], 7);                     \
    a.x[0] = a.x[0] ^ a.x[1] ^ a.x[3];           \
    a.x[2] = a.x[2] ^ a.x[3] ^ (a.x[1] << 7);    \
    a.x[0] = rL(a.x[0], 5);                     \
    a.x[2] = rL(a.x[2], 22);                    \
  }



#define SBOX0(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S0(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX1(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S1(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX2(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S2(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX3(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S3(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX4(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S4(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX5(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S5(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX6(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S6(s0[i], s1[i], s2[i], s3[i]);   \
}
#define SBOX7(s0, s1, s2, s3)             \
{                                         \
    for (int i = 0; i < 32; i++)          \
        S7(s0[i], s1[i], s2[i], s3[i]);   \
}




#define SBOX_CASE(which, s0, s1, s2, s3)   \
    switch (which) {                       \
    case 0: SBOX0(s0, s1, s2, s3); break;  \
    case 1: SBOX1(s0, s1, s2, s3); break;  \
    case 2: SBOX2(s0, s1, s2, s3); break;  \
    case 3: SBOX3(s0, s1, s2, s3); break;  \
    case 4: SBOX4(s0, s1, s2, s3); break;  \
    case 5: SBOX5(s0, s1, s2, s3); break;  \
    case 6: SBOX6(s0, s1, s2, s3); break;  \
    case 7: SBOX7(s0, s1, s2, s3); break;  \
    }                                            





bool MySort(PolyTerm a, PolyTerm b)
{
    for (int i = 0; i < 128; i++)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
    }
    for (int i = loc_1 + 1; i < VARIABLE_COUNT; i++)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
    }
    if (a[loc_x] > b[loc_x])
        return true;
    else if (a[loc_x] < b[loc_x])
        return false;

    if (a[loc_1] > b[loc_1])
        return true;
    else if (a[loc_1] < b[loc_1])
        return false;

    return false;
}
Poly multiply(Poly a, Poly b)
{
    Poly result;
    unordered_map<PolyTerm, int> occurrences;
    for (auto& termA : a)
    {
        for (auto& termB : b)
        {
            PolyTerm newTerm = termA | termB;
            if (newTerm[loc_1] == 1 && newTerm.count() > 1)
                newTerm[loc_1] = 0;
            occurrences[newTerm]++;
        }
    }
    for (auto& pair : occurrences)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);
    sort(result.begin(), result.end(), MySort);
    return result;
}
Poly add(Poly a, Poly b)
{
    Poly result;
    unordered_map<PolyTerm, int> termCount;

    for (auto& term : a)
        termCount[term]++;

    for (auto& term : b)
        termCount[term]++;

    for (const auto& pair : termCount)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);

    sort(result.begin(), result.end(), MySort);
    return result;
}
Poly addmore(vector<Poly> polys)
{
    Poly result;
    for (auto& poly : polys)
        result.insert(result.end(), poly.begin(), poly.end());

    unordered_map<PolyTerm, int> occurrences;
    for (auto& term : result)
        occurrences[term]++;

    result.clear();
    for (auto& pair : occurrences)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);

    sort(result.begin(), result.end(), MySort);
    return result;
}
Poly add_one(Poly a)
{
    sort(a.begin(), a.end(), MySort);

    PolyTerm term;
    term.set(loc_1);
    if (a.size() == 0)
    {
        Poly one;
        one.push_back(term);
        return one;
    }
    else
    {
        if (a[size(a) - 1][loc_1] == 1)
            a.pop_back();
        else
            a.push_back(term);
        return a;
    }

}



Poly calculatedifference(Poly poly)
{
    Poly difference;
    for (auto& term : poly)
    {
        if (term[loc_x] == 1)
        {
            term.reset(loc_x);
            if (term.count() == 0)
                term.set(loc_1);
            difference.push_back(term);
        }
    }
    return difference;
}
void DATF(Poly poly, Poly& a, Poly& b)
{
    for (auto& term : poly)
    {
        if (term[loc_x] == 1)
        {
            term.reset(loc_x);
            if (term.count() == 0)
                term.set(loc_1);
            b.push_back(term);
        }
        else
            a.push_back(term);
    }
}
void printPoly(Poly poly)
{
    int i;
    for (auto& term : poly)
    {
        for (i = 0; i < 128; i++)
            if (term[i] == 1)   cout << "v" << i;
        for (i = 0; i < 128 * round; i++)
            if (term[i + loc_1 + 1] == 1)   cout << "a" << i;
        for (i = 0; i < 128 * round; i++)
            if (term[i + loc_1 + 1 + 128 * round] == 1)   cout << "b" << i;
        if (term[loc_x] == 1)   cout << "x";
        if (term[loc_1] == 1)   cout << "1";
        cout << "+";
    }
    cout << endl;
}
void printDATF_file(vector<Poly>& A, vector<Poly>& B)
{
    int i, loc;
    ofstream outfile;
    outfile.open("DATF.txt");

    for (loc = 0; loc < 128 * round; loc++)
    {
        sort(A[loc].begin(), A[loc].end(), MySort);
        outfile << "a" << loc << ":";
        for (auto& term : A[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "v" << i;
            for (i = 0; i < 128 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 128 * round; i++)
                if (term[i + loc_1 + 1 + 128 * round] == 1)   outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < 128 * round; loc++)
    {
        sort(B[loc].begin(), B[loc].end(), MySort);
        outfile << "b" << loc << ":";
        for (auto& term : B[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "v" << i;
            for (i = 0; i < 128 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 128 * round; i++)
                if (term[i + loc_1 + 1 + 128 * round] == 1)   outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;


    outfile.close();
}


void S0(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y0 = 1 ^ x0 ^ x0x1 ^ x2 ^ x0x2 ^ x1x2 ^ x0x1x2 ^ x3 ^ x0x2x3 ^ x1x2x3
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y0 = add_one(addmore(T));

    //y1 = 1 ^ x0 ^ x0x2 ^ x1x2 ^ x0x1x2 ^ x1x3 ^ x0x2x3 ^ x1x2x3
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y1 = add_one(addmore(T));

    //y2 = x1 ^ x0x1 ^ x0x2 ^ x0x1x2 ^ x3 ^ x1x3 ^ x1x2x3
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x2), x1));
    y2 = addmore(T);

    //y3 = x0 ^ x1 ^ x2 ^ x3 ^ x0x3
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    y3 = addmore(T);

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S1(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y0 = 1 ^ x0 ^ x1 ^ x1x2 ^ x0x3 ^ x2x3 ^ x0x2x3 ^ x1x2x3
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y0 = add_one(addmore(T));

    //y1 = 1 ^ x0 ^ x0x1 ^ x2 ^ x0x2 ^ x3 ^ x1x3 ^ x0x1x3 ^ x0x2x3 ^ x1x2x3
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y1 = add_one(addmore(T));

    //y2 = 1 ^ x1 ^ x0x1 ^ x2 ^ x3
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(x3);
    y2 = add_one(addmore(T));

    //y3 = 1 ^ x1 ^ x0x2 ^ x3 ^ x0x3 ^ x0x1x3 ^ x0x2x3 ^ x1x2x3
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x2, x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y3 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S2(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y0 = x1 ^ x2 ^ x0x2 ^ x3
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(x3);
    y0 = addmore(T);

    //y1 = x0 ^ x1 ^ x2 ^ x1x2 ^ x0x1x2 ^ x0x3 ^ x0x1x3 ^ x2x3 ^ x0x2x3
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    y1 = addmore(T);

    //y2 = x0 ^ x1 ^ x1x2 ^ x3 ^ x1x3 ^ x0x1x3 ^ x2x3 ^ x0x2x3
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(multiply(x2, x1));
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    y2 = addmore(T);

    //y3 = 1 ^ x0 ^ x1 ^ x2 ^ x0x1x2 ^ x1x3
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x1));
    y3 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S3(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y3 : x0 + x1 + x1x0 + x2 + x2x0 + x2x1x0 + x3 + x3x2 + x3x2x0
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    y3 = addmore(T);

    //y2 : x0 + x1x0 + x2 + x2x1x0 + x3 + x3x1 + x3x1x0
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    y2 = addmore(T);

    //y1 : x0 + x1 + x2x0 + x3x0 + x3x1x0 + x3x2 + x3x2x0
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    y1 = addmore(T);

    //y0 : x0 + x1 + x2x1 + x3 + x3x0 + x3x2 + x3x2x0 + x3x2x1
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(multiply(x2, x1));
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y0 = addmore(T);
    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S4(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y3: x0 + x1 + x2 + x2x1 + x3x0 + x3x1 + x3x1x0
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    y3 = addmore(T);

    //y2 : x0 + x1x0 + x2 + x2x1 + x2x1x0 + x3x1 + x3x1x0 + x3x2 + x3x2x1
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x1));
    y2 = addmore(T);

    //y1 : x0 + x2x0 + x2x1 + x3 + x3x1 + x3x2 + x3x2x0 + x3x2x1
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x2, x1));
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y1 = addmore(T);

    //y0 : 1 + x1 + x1x0 + x2 + x3 + x3x0 + x3x1
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x1));
    y0 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S5(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y3: 1 + x0 + x1 + x2 + x2x1x0 + x3 + x3x0 + x3x2x0
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(multiply(x3, x2), x0));
    y3 = add_one(addmore(T));

    //y2 : 1 + x1 + x2x0 + x3 + x3x1x0 + x3x2 + x3x2x0 + x3x2x1
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x2, x0));
    T.push_back(x3);
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y2 = add_one(addmore(T));

    //y1 : 1 + x0 + x1x0 + x2 + x3 + x3x1 + x3x1x0 + x3x2
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(x3);
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    y1 = add_one(addmore(T));

    //y0 : 1 + x1 + x1x0 + x2 + x3 + x3x0 + x3x1
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x1));
    y0 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S6(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y3: x1 ^ x1*x0 ^ x2 ^ x2*x0 ^ x2*x1*x0 ^ x3 ^ x3*x2 ^ x3*x2*x1
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x1));
    y3 = addmore(T);

    //y2 : 1 ^ x0 ^ x1*x0 ^ x2 ^ x2*x1 ^ x2*x1*x0 ^ x3*x1 ^ x3*x1*x0 ^ x3*x2 ^ x3*x2*x1
    T.clear();
    T.push_back(x0);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x1));
    y2 = add_one(addmore(T));

    //y1 : 1 ^ x1 ^ x2 ^ x3*x0
    T.clear();
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x3, x0));
    y1 = add_one(addmore(T));

    //y0 : 1 ^ x0 ^ x1 ^ x2 ^ x2*x0 ^ x2*x1 ^ x2*x1*x0 ^ x3 ^ x3*x1*x0 ^ x3*x2*x1
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x2, x1));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y0 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}
void S7(Poly& x0, Poly& x1, Poly& x2, Poly& x3)
{
    Poly y0, y1, y2, y3;
    vector<Poly> T;

    //y3: x0 ^ x1 ^ x2 ^ x2x0 ^ x2x1x0 ^ x3x0
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(multiply(x3, x0));
    y3 = addmore(T);

    //y2 : x0 ^ x1 ^ x2 ^ x2x1x0 ^ x3 ^ x3x0 ^ x3x1 ^ x3x1x0 ^ x3x2x1
    T.clear();
    T.push_back(x0);
    T.push_back(x1);
    T.push_back(x2);
    T.push_back(multiply(multiply(x2, x1), x0));
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y2 = addmore(T);

    //y1 : x1  ^ x1x0 ^ x2 ^ x2x0 ^ x2x1 ^ x3 ^ x3x0 ^ x3x1x0 ^ x3x2x0
    T.clear();
    T.push_back(x1);
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x2, x0));
    T.push_back(multiply(x2, x1));
    T.push_back(x3);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(multiply(x3, x1), x0));
    T.push_back(multiply(multiply(x3, x2), x0));
    y1 = addmore(T);

    //y0 : 1 ^ x1*x0 ^ x2 ^ x3*x0 ^ x3*x1 ^ x3*x2 ^ x3*x2*x0 ^ x3*x2*x1
    T.clear();
    T.push_back(multiply(x1, x0));
    T.push_back(x2);
    T.push_back(multiply(x3, x0));
    T.push_back(multiply(x3, x1));
    T.push_back(multiply(x3, x2));
    T.push_back(multiply(multiply(x3, x2), x0));
    T.push_back(multiply(multiply(x3, x2), x1));
    y0 = add_one(addmore(T));

    x0 = y0;
    x1 = y1;
    x2 = y2;
    x3 = y3;
}



vector<Poly> rotateLeft(vector<Poly> x, int r)
{
    vector<Poly> y(32);
    for (int i = 0; i < 32; i++)
        y[i] = x[(i + r) % 32];
    return y;
}
vector<Poly> shiftLeft(vector<Poly> x, int r)
{
    vector<Poly> y(32);
    for (int i = 0; i < 32 - r; i++)
        y[i] = x[i + r];
    return y;
}
vector<Poly> add_3word(vector<Poly> x0, vector<Poly> x1, vector<Poly> x2)
{
    vector<Poly> y(32);
    for (int i = 0; i < 32; i++)
    {
        vector<Poly> vec = { x0[i],x1[i],x2[i] };
        y[i] = addmore(vec);
    }
    return y;
}
void LT(vector<Poly>& s0, vector<Poly>& s1, vector<Poly>& s2, vector<Poly>& s3)
{
    s0 = rotateLeft(s0, 13);
    s2 = rotateLeft(s2, 3);
    s1 = add_3word(s0, s1, s2);
    s3 = add_3word(s3, s2, shiftLeft(s0, 3));
    s1 = rotateLeft(s1, 1);
    s3 = rotateLeft(s3, 7);
    s0 = add_3word(s0, s1, s3);
    s2 = add_3word(s2, s3, shiftLeft(s1, 7));
    s0 = rotateLeft(s0, 5);
    s2 = rotateLeft(s2, 22);
}


string polyToString(Poly poly)
{
    string result;
    sort(poly.begin(), poly.end(), MySort);
    for (const auto& term : poly)
    {
        result += term.to_string();
    }
    return result;
}
struct PolyCompare
{
    bool operator()(const Poly& a, const Poly& b) const
    {
        return polyToString(a) < polyToString(b);
    }
};

bool isReplace(Poly poly)
{

    sort(poly.begin(), poly.end(), MySort);
    PolyTerm variable;
    for (auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;
    if (variable.count() <= 3)
        return false;
    if (poly.size() == 1)
        return false;
    else if (poly.size() == 2 && poly[1][loc_1] == 1)
        return false;
    for (auto& term : poly)
        if (term.size() > 1)
            return true;

    return false;

}
bool isInverse(const Poly& a, const Poly& b)
{
    PolyTerm constant1_term;
    constant1_term.set(loc_1);
    Poly plus = add(a, b);
    return plus[0] == constant1_term;
}
void ProcessingDATF(int r, vector<Poly>& s0, vector<Poly>& s1, vector<Poly>& s2, vector<Poly>& s3, vector<Poly>& A, vector<Poly>& B)
{
    int flag;
    Poly poly;
    PolyTerm term;
    map<Poly, int, PolyCompare> polyToVarB;

    for (int i = 0; i < 32; i++)
    {
        DATF(s0[i], A[r * 128 + i], B[r * 128 + i]);

        poly.clear();
        term.reset();
        term.set(loc_1 + 1 + 128 * r + i);
        poly.push_back(term);



        if (isReplace(B[r * 128 + i]))
        {
            term.reset();
            term.set(loc_x);
            auto it = polyToVarB.find(B[r * 128 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 128 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 128 + i]))
                    {

                        term.set(polyToVarB[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[r * 128 + i]] = loc_1 + 1 + 128 * round + 128 * r + i;
                    term.set(loc_1 + 1 + 128 * round + 128 * r + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto t : B[r * 128 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s0[i] = poly;
    }


    for (int i = 0; i < 32; i++)
    {
        DATF(s1[i], A[r * 128 + 32 + i], B[r * 128 + 32 + i]);

        if (isReplace(A[r * 128 + 32 + i]))
        {
            poly.clear();
            term.reset();
            term.set(loc_1 + 1 + 128 * r + 32 + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 128 + 32 + i];


        if (isReplace(B[r * 128 + 32 + i]))
        {
            term.reset();
            term.set(loc_x);
            auto it = polyToVarB.find(B[r * 128 + 32 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 128 + 32 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 128 + 32 + i]))
                    {
                        term.set(polyToVarB[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[r * 128 + 32 + i]] = loc_1 + 1 + 128 * round + 128 * r + 32 + i;
                    term.set(loc_1 + 1 + 128 * round + 128 * r + 32 + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto t : B[r * 128 + 32 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s1[i] = poly;
    }


    for (int i = 0; i < 32; i++)
    {
        DATF(s2[i], A[r * 128 + 64 + i], B[r * 128 + 64 + i]);

        if (isReplace(A[r * 128 + 64 + i]))
        {
            poly.clear();
            term.reset();
            term.set(loc_1 + 1 + 128 * r + 64 + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 128 + 64 + i];



        if (isReplace(B[r * 128 + 64 + i]))
        {
            term.reset();
            term.set(loc_x);
            auto it = polyToVarB.find(B[r * 128 + 64 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 128 + 64 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 128 + 64 + i]))
                    {
                        term.set(polyToVarB[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[r * 128 + 64 + i]] = loc_1 + 1 + 128 * round + 128 * r + 64 + i;
                    term.set(loc_1 + 1 + 128 * round + 128 * r + 64 + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto t : B[r * 128 + 64 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s2[i] = poly;
    }



    for (int i = 0; i < 32; i++)
    {
        DATF(s3[i], A[r * 128 + 96 + i], B[r * 128 + 96 + i]);

        if (isReplace(A[r * 128 + 96 + i]))
        {
            poly.clear();
            term.reset();
            term.set(loc_1 + 1 + 128 * r + 96 + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 128 + 96 + i];



        if (isReplace(B[r * 128 + 96 + i]))
        {
            term.reset();
            term.set(loc_x);
            auto it = polyToVarB.find(B[r * 128 + 96 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 128 + 96 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 128 + 96 + i]))
                    {
                        term.set(polyToVarB[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[r * 128 + 96 + i]] = loc_1 + 1 + 128 * round + 128 * r + 96 + i;
                    term.set(loc_1 + 1 + 128 * round + 128 * r + 96 + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto t : B[r * 128 + 96 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s3[i] = poly;
    }

}


vector<int> countAndSortVariables(vector<Poly> allpolys, int subset_size)
{
    vector<int> counts(128, 0);

    for (auto& poly : allpolys)
        for (const auto& term : poly)
            for (size_t i = 0; i < 128; ++i)
            {
                if (term.test(i))
                    counts[i]++;
            }

    vector<std::pair<int, int>> var_counts;
    for (int i = 0; i < 128; ++i)
        var_counts.emplace_back(i, counts[i]);


    sort(var_counts.begin(), var_counts.end(), [](const auto& a, const auto& b) {
        return a.second > b.second || (a.second == b.second && a.first < b.first);
        });

    vector<int> sorted_indices;

    for (int i = 0; i < subset_size; ++i)
    {
        if (var_counts[i].second > 0)
        {
            // cout << var_counts[i].first << ": " << var_counts[i].second << endl;
            sorted_indices.push_back(var_counts[i].first);
        }
    }


    return sorted_indices;
}

vector<int> calculate_subset(int subset_size, int offset, vector<int> delta, vector<Poly>& s0, vector<Poly>& s1, vector<Poly>& s2, vector<Poly>& s3)
{
    int i;

    bitset<128> IV(0);
    for (int value : delta)
        IV.set(value);


    Poly poly;
    PolyTerm term;
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        term.reset();
        term.set(i);
        poly.push_back(term);
        if (IV[i] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s0[i] = poly;
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        term.reset();
        term.set(i + 32);
        poly.push_back(term);
        if (IV[i + 32] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s1[i] = poly;
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        term.reset();
        term.set(i + 64);
        poly.push_back(term);
        if (IV[i + 64] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s2[i] = poly;
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        term.reset();
        term.set(i + 96);
        poly.push_back(term);
        if (IV[i + 96] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s3[i] = poly;
    }
    SBOX_CASE(offset, s0, s1, s2, s3);
    LT(s0, s1, s2, s3);


    vector<int> subset_IV;

    int count[128] = { 0 };



    vector<Poly> tt;
    for (i = 0; i < 32; i++)
    {
        tt.push_back(calculatedifference(s0[i]));
        tt.push_back(calculatedifference(s1[i]));
        tt.push_back(calculatedifference(s2[i]));
        tt.push_back(calculatedifference(s3[i]));
    }

    return countAndSortVariables(tt, subset_size);
}

Poly calculateANF(int r, int offset, vector<int> delta, vector<int> lambda, vector<int> subset_IV, int loc, vector<Poly>& s0, vector<Poly>& s1, vector<Poly>& s2, vector<Poly>& s3, vector<Poly>& A, vector<Poly>& B)
{
    int i;

    bitset<128> IV(0);
    for (int value : delta)
        IV.set(value);

    bitset<128> sub(0);
    for (int value : subset_IV)
        sub.set(value);

    int index = 0;
    Poly poly;
    PolyTerm term;
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        if (IV[i] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        if (sub[i] == 0)
        {
            term.reset();
            term.set(i);
            poly.push_back(term);
        }
        else
        {
            if ((loc >> index) & 1)
            {
                term.reset();
                term.set(loc_1);
                poly.push_back(term);
            }
            index++;
        }
        s0[i] = poly;
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        if (IV[i + 32] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        if (sub[i + 32] == 0)
        {
            term.reset();
            term.set(i + 32);
            poly.push_back(term);
        }
        else
        {
            if ((loc >> index) & 1)
            {
                term.reset();
                term.set(loc_1);
                poly.push_back(term);
            }
            index++;
        }
        s1[i] = poly;
        // printPoly(s1[i]);
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        if (IV[i + 64] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        if (sub[i + 64] == 0)
        {
            term.reset();
            term.set(i + 64);
            poly.push_back(term);
        }
        else
        {
            if ((loc >> index) & 1)
            {
                term.reset();
                term.set(loc_1);
                poly.push_back(term);
            }
            index++;
        }
        s2[i] = poly;
        // printPoly(s2[i]);
    }
    for (i = 0; i < 32; i++)
    {
        poly.clear();
        if (IV[i + 96] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        if (sub[i + 96] == 0)
        {
            term.reset();
            term.set(i + 96);
            poly.push_back(term);
        }
        else
        {
            if ((loc >> index) & 1)
            {
                term.reset();
                term.set(loc_1);
                poly.push_back(term);
            }
            index++;
        }
        s3[i] = poly;

    }
    for (int loc = 0; loc < r; loc++)
    {
        SBOX_CASE((offset + loc) % 8, s0, s1, s2, s3);
        LT(s0, s1, s2, s3);

        if (loc < r - 1)
            ProcessingDATF(loc, s0, s1, s2, s3, A, B);
    }

    vector<Poly> T;
    bitset<128> lamb(0);
    for (i = 0; i < lambda.size(); i++)
        lamb.set(lambda[i]);

    for (i = 0; i < 32; i++)
    {
        if (lamb[i])  T.push_back(s0[i]);
        if (lamb[i + 32])  T.push_back(s1[i]);
        if (lamb[i + 64])  T.push_back(s2[i]);
        if (lamb[i + 96])  T.push_back(s3[i]);
    }

    return calculatedifference(addmore(T));

}


vector<Poly> calculateSeparate(Poly poly)
{
    vector<Poly> Separate;
    int i;
    Poly poly1;
    PolyTerm variable;

    int size = poly.size();
    int* flag = new int[size];
    for (i = 0; i < size; i++)
        flag[i] = 0;

    for (i = 0; i < size + 1; i++)
    {
        if (i == size)
        {
            if (poly1.size() == 0)
                break;
            Separate.push_back(poly1);
            poly1.clear();
            i = 0;
        }
        if (flag[i] == 0)
        {
            if (poly1.size() == 0)
            {
                poly1.push_back(poly[i]);
                variable = poly[i];
                flag[i] = 1;
            }
            else
            {
                if ((variable & poly[i]).any())
                {
                    poly1.push_back(poly[i]);
                    variable = variable | poly[i];
                    flag[i] = 1;
                    i = 0;
                }
            }
        }
    }
    delete[] flag;

    return Separate;
}
int cardinality(Poly poly)
{
    int i, maxx = 0;
    PolyTerm variable;
    vector<Poly> Separate = calculateSeparate(poly);
    for (i = 0; i < Separate.size(); i++)
    {
        variable.reset();
        for (auto term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;
        if (variable.count() > maxx)
            maxx = variable.count();
    }
    return maxx;
}
Poly simplify_f(int flag, Poly f, int value)
{
    Poly poly;
    if (value == 0)
    {
        for (const auto& term : f)
        {
            if (term[flag] == 0)
                poly.push_back(term);
        }

    }
    if (value == 1)
    {
        for (auto term : f)
        {
            if (term[flag] == 1)
            {
                term.reset(flag);
                if (term.count() == 0)
                    term.set(loc_1);
                poly.push_back(term);
            }
            else
                poly.push_back(term);
        }
        unordered_map<PolyTerm, int> occurrences;
        for (const auto& tt : poly)
            occurrences[tt]++;

        poly.clear();
        for (const auto& pair : occurrences)
            if (pair.second % 2 == 1)
                poly.push_back(pair.first);
    }
    return poly;
}
int calculatevalue(Poly poly, PolyTerm term, vector<int> variables, int size)
{
    int value = 0;
    int flag;
    for (const auto& tttt : poly)
    {
        if (tttt[loc_1] == 1)
            value = value ^ 1;
        else
        {
            for (int i = 0; i < size; i++)
            {
                if (tttt[variables[i]] == 1)
                {
                    if (term[variables[i]] == 1)
                        flag = 1;
                    else
                    {
                        flag = 0;
                        break;
                    }
                }
            }
            if (flag == 1)
                value = value ^ 1;
        }
    }
    return value;
}



double calculatebias_v1(Poly poly, vector<double>& B_bias)
{
    PolyTerm variable;
    for (const auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;

    vector<int> variables;
    for (int i = 0; i < VARIABLE_COUNT; i++)
        if (variable[i] == 1)
            variables.push_back(i);
    int size = variables.size();


    double P0 = 0;
    for (long long j = 0; j < pow(2, size); j++)
    {
        PolyTerm term(0);
        for (int k = 0; k < size; k++)
            term[variables[k]] = (j >> k) & 1;
        if (calculatevalue(poly, term, variables, size) == 0)
        {
            double prob = 1.0;
            for (int k = 0; k < size; k++)
            {
                if (term[variables[k]] == 1)
                {
                    if (variables[k] < loc_1 + 1 + 128 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[variables[k] - loc_1 - 1 - 128 * round]);
                }
                else
                {
                    if (variables[k] < loc_1 + 1 + 128 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[variables[k] - loc_1 - 1 - 128 * round]);
                }
            }
            P0 = P0 + prob;
        }
    }

    return P0 - 0.5;
}
int simplifiedcalculatebias(Poly poly, vector<double>& B_bias)
{
    vector<Poly> Separate = calculateSeparate(poly);

    for (int i = 0; i < Separate.size(); i++)
    {
        PolyTerm variable;
        for (const auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        if (variable.count() < 5)
            if (calculatebias_v1(Separate[i], B_bias) == 0)
                return 0;
    }
    return 1;
}
double preprocess(Poly& poly, vector<double>& B_bias)
{
    //cout << "Pretreat." << endl;
    double prob = 1.0;
    int flag = 1;
    while (flag == 1)
    {
        flag = 0;
        PolyTerm variable;
        for (const auto& term : poly)
            variable = variable | term;
        variable[loc_1] = 0;

        Poly f0, f1;
        double b0, b1;
        for (int loc = 0; loc < VARIABLE_COUNT; loc++)
        {
            if (variable[loc] == 1)
            {
                f0 = simplify_f(loc, poly, 0);
                f1 = simplify_f(loc, poly, 1);
                b0 = simplifiedcalculatebias(f0, B_bias);
                b1 = simplifiedcalculatebias(f1, B_bias);
                if (b0 == 0 && b1 == 0)
                    return -1;
                else if (b0 == 0)
                {
                    poly = f1;
                    if (loc < loc_1 + 1 + 128 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[loc - loc_1 - 1 - 128 * round]);
                    flag = 1;
                    break;
                }
                else if (b1 == 0)
                {
                    poly = f0;
                    if (loc < loc_1 + 1 + 128 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[loc - loc_1 - 1 - 128 * round]);
                    flag = 1;
                    break;
                }
            }
        }
    }
    return prob;
}


double calculatebias_v2(Poly poly, vector<double>& B_bias)
{
    double bias;

    vector<Poly> Separate = calculateSeparate(poly);

    double epsilon = 0.5;
    for (int i = 0; i < Separate.size(); i++)
    {
        PolyTerm variable;
        for (const auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        if (variable.count() <= 10)
            epsilon = 2 * epsilon * calculatebias_v1(Separate[i], B_bias);   //Exhaustive computation
        else
        {
            double preprocess_p = preprocess(Separate[i], B_bias);
            if (preprocess_p >= 0)
            {
                if (Separate[i].size() == 1 && Separate[i][0][loc_1] == 1)
                    bias = -0.5;
                else
                {
                    variable.reset();
                    for (const auto& term : Separate[i])
                        variable = variable | term;
                    variable[loc_1] = 0;

                    int minn = 200, min_temp, loc;
                    Poly f0, f1, h0, h1;
                    for (int j = 0; j < VARIABLE_COUNT; j++)
                    {
                        if (variable[j] == 1)
                        {
                            f0 = simplify_f(j, Separate[i], 0);
                            f1 = simplify_f(j, Separate[i], 1);
                            min_temp = max(cardinality(f0), cardinality(f1));
                            if (min_temp < minn)
                            {
                                minn = min_temp;
                                h0 = f0;
                                h1 = f1;
                                loc = j;
                            }
                        }
                    }
                    double p_0;
                    if (loc < loc_1 + 1 + 128 * round)
                        p_0 = 0.5;
                    else
                        p_0 = 0.5 + B_bias[loc - loc_1 - 1 - 128 * round];

                    if (simplifiedcalculatebias(h0, B_bias) == 1)
                    {
                        if (simplifiedcalculatebias(h1, B_bias) == 1)
                            bias = p_0 * calculatebias_v2(h0, B_bias) + (1 - p_0) * calculatebias_v2(h1, B_bias);
                        else
                            bias = p_0 * (calculatebias_v2(h0, B_bias));
                    }
                    else
                    {
                        if (simplifiedcalculatebias(h1, B_bias) == 1)
                            bias = (1 - p_0) * (calculatebias_v2(h1, B_bias));
                        else
                            bias = 0;
                    }
                }
                bias = preprocess_p * bias;
                epsilon = 2 * epsilon * bias;
            }
            else if (preprocess_p == -1)
            {
                bias = 0;
                epsilon = 0;
            }
        }
        if (epsilon == 0)
            return 0;
    }
    return epsilon;
}

void calculateBbias(int r, vector<Poly> B, vector<double>& B_bias)
{
    // cout << "----------------  Calculate B bias  ----------------" << endl;
    for (int i = 0; i < 128 * r; i++)
    {
        if (B[i].size() == 0)
            B_bias[i] = 0.5;
        else if (B[i].size() == 1 && B[i][0][loc_1] == 1)
            B_bias[i] = -0.5;
        else
            B_bias[i] = calculatebias_v2(B[i], B_bias);

        //  cout << i << ": " << B_bias[i] << endl;
    }
}

int find_isolate(Poly poly)
{
    int flag;
    for (int i = round - 1; i >= 0; i--)
    {

        for (int j = 0; j < 128; j++)
        {

            flag = 0;
            for (auto term : poly)
            {
                if (term[loc_1 + 1 + 128 * round + 128 * i + j] == 1)
                {
                    flag = flag + term.count();
                    if (flag > 1)
                        break;
                }
            }
            if (flag == 1)
                return loc_1 + 1 + 128 * round + 128 * i + j;


            flag = 0;
            for (auto term : poly)
            {
                if (term[loc_1 + 1 + 128 * i + j] == 1)
                {
                    flag = flag + term.count();
                    if (flag > 1)
                        break;
                }
            }
            if (flag == 1)
                return loc_1 + 1 + 128 * i + j;

        }
    }
    return -1;
}
Poly pushback(Poly difference, vector<Poly>& A, vector<Poly>& B)
{
    int isolate;
    Poly poly, condition;
    //cout << "pushback " << endl;
    isolate = find_isolate(difference);
    while (isolate >= 0)
    {
        if (isolate < loc_1 + 1 + 128 * round)
            condition = A[isolate - (loc_1 + 1)];
        else
            condition = B[isolate - (loc_1 + 1) - 128 * round];


        poly.clear();

        for (auto term : difference)
        {
            if (term[isolate] != 1)
                poly.push_back(term);
        }
        difference = add(poly, condition);
        isolate = find_isolate(difference);
        //printPoly(difference1);
        //cout << endl;
    }
    //cout << endl;
    return difference;
}



void DL1()
{

    int r = 2;
    int offset = 3;
    vector<int> delta = { 15, 21, 58, 93, 94, 116 };
    vector<int> lambda = { 63,66 };


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


    vector<Poly> s0(32), s1(32), s2(32), s3(32);
    vector<Poly> A(128 * r), B(128 * r);

    vector<int> subset = {};
    Poly difference = calculateANF(r, offset, delta, lambda, subset, 0, s0, s1, s2, s3, A, B);
    vector<double> B_bias(128 * r);
    calculateBbias(r, B, B_bias);

    double bias = calculatebias_v2(difference, B_bias);
    cout << "bias: " << bias << endl;
    cout << endl;
}


void DL2()
{
    int r = 3;
    int offset = 2;
    vector<int> delta = { 78 };
    vector<int> lambda = { 61,64 };

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



    int subset_size = 10;


    vector<Poly> s0(32), s1(32), s2(32), s3(32);
    vector<Poly> A(128 * round), B(128 * round);

    vector<int> subset = calculate_subset(subset_size, offset, delta, s0, s1, s2, s3);

    cout << "subset:  ";
    for (int num : subset)
    {
        cout << num << ", ";
    }
    cout << endl;

    double bias = 0;
#pragma omp parallel for
    for (int loc = 0; loc < (1 << subset.size()); loc++)
    {
        vector<Poly> s0(32), s1(32), s2(32), s3(32);
        vector<Poly> A(128 * r), B(128 * r);

        Poly difference = calculateANF(r, offset, delta, lambda, subset, loc, s0, s1, s2, s3, A, B);

        vector<double> B_bias(128 * r);
        calculateBbias(r, B, B_bias);

        double b_temp = calculatebias_v2(difference, B_bias);

#pragma omp critical
        {
            bias += b_temp;
        }


    };
    cout << "bias: " << bias / (1 << subset.size()) << endl;
    cout << endl;
}



void DL3()
{

    int r = 3;
    int offset = 2;
    vector<int> delta = { 27 };
    vector<int> lambda = { 63,66 };

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



    int subset_size = 10;


    vector<Poly> s0(32), s1(32), s2(32), s3(32);
    vector<Poly> A(128 * round), B(128 * round);

    vector<int> subset = calculate_subset(subset_size, offset, delta, s0, s1, s2, s3);

    cout << "subset:  ";
    for (int num : subset)
    {
        cout << num << ", ";
    }
    cout << endl;

    double bias = 0;
#pragma omp parallel for
    for (int loc = 0; loc < (1 << subset.size()); loc++)
    {
        vector<Poly> s0(32), s1(32), s2(32), s3(32);
        vector<Poly> A(128 * r), B(128 * r);

        Poly difference = calculateANF(r, offset, delta, lambda, subset, loc, s0, s1, s2, s3, A, B);

        vector<double> B_bias(128 * r);
        calculateBbias(r, B, B_bias);

        double b_temp = calculatebias_v2(difference, B_bias);

#pragma omp critical
        {
            bias += b_temp;
        }


    };
    cout << "bias: " << bias / (1 << subset.size()) << endl;
    cout << endl;
}


void DL4()
{

    int r = 3;
    int offset = 3;
    vector<int> delta = { 83 };
    vector<int> lambda = { 119 };

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



    int subset_size = 10;


    vector<Poly> s0(32), s1(32), s2(32), s3(32);
    vector<Poly> A(128 * round), B(128 * round);

    vector<int> subset = calculate_subset(subset_size, offset, delta, s0, s1, s2, s3);

    cout << "subset:  ";
    for (int num : subset)
    {
        cout << num << ", ";
    }
    cout << endl;

    double bias = 0;
#pragma omp parallel for
    for (int loc = 0; loc < (1 << subset.size()); loc++)
    {
        vector<Poly> s0(32), s1(32), s2(32), s3(32);
        vector<Poly> A(128 * r), B(128 * r);

        Poly difference = calculateANF(r, offset, delta, lambda, subset, loc, s0, s1, s2, s3, A, B);

        vector<double> B_bias(128 * r);
        calculateBbias(r, B, B_bias);

        double b_temp = calculatebias_v2(difference, B_bias);

#pragma omp critical
        {
            bias += b_temp;
        }


    };
    cout << "bias: " << bias / (1 << subset.size()) << endl;
    cout << endl;
}





int main()
{

    DL1();       // 0.0078125
    DL2();       // -0.00242615
    DL3();       // 0.00100708
    DL4();       //  -0.0065155

    system("pause");
    return 0;
}

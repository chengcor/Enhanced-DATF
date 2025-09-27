#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <map>
#include <time.h>
#include <string>
#define round 5
using namespace std;


const int N0 = 2;
const int VARIABLE_COUNT = 128 + 128 + 1 + 1 + 640 * round;       // key(128) + IV(128) + x + const 1 + ATF
const int loc_x = 256;
const int loc_1 = 257;
using PolyTerm = bitset<VARIABLE_COUNT>;
using Poly = vector<PolyTerm>;

//   Part 1: Output by Format
string polyToString(const Poly& poly)
{
    string result;
    for (const auto& term : poly)
    {
        result += term.to_string();
        result += " ";
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
bool MySort(PolyTerm a, PolyTerm b)
{
    int i;
    for (i = 0; i < loc_x; i++)
    {
        if (a[i] > b[i])
            return true;
        else if (a[i] < b[i])
            return false;
    }
    for (i = loc_1 + 1; i < VARIABLE_COUNT; i++)
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
void printPoly(Poly poly)
{
    int i;
    for (auto& term : poly)
    {
        for (i = 0; i < 128; i++)
            if (term[i] == 1)   cout << "k" << i;
        for (i = 0; i < 128; i++)
            if (term[i + 128] == 1) cout << "v" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1] == 1)   cout << "a" << i;
        for (i = 0; i < 320 * round; i++)
            if (term[i + loc_1 + 1 + 320 * round] == 1) cout << "b" << i;
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

    for (loc = 0; loc < 320 * round; loc++)
    {
        sort(A[loc].begin(), A[loc].end(), MySort);
        outfile << "a" << loc << ":";
        for (auto& term : A[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "k" << i;
            for (i = 0; i < 128; i++)
                if (term[i + 128] == 1) outfile << "v" << i;
            for (i = 0; i < 320 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 320 * round; i++)
                if (term[i + loc_1 + 1 + 320 * round] == 1) outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < 320 * round; loc++)
    {
        sort(B[loc].begin(), B[loc].end(), MySort);
        outfile << "b" << loc << ":";
        for (auto& term : B[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "k" << i;
            for (i = 0; i < 128; i++)
                if (term[i + 128] == 1) outfile << "v" << i;
            for (i = 0; i < 320 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 320 * round; i++)
                if (term[i + loc_1 + 1 + 320 * round] == 1) outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    outfile.close();
}

//   Part 2: Calculating the DATF
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
        if (a[a.size() - 1][loc_1] == 1)
            a.pop_back();
        else
            a.push_back(term);
        return a;
    }
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
bool isInverse(const Poly& a, const Poly& b)
{
    PolyTerm constant1_term;
    constant1_term.set(loc_1);
    Poly plus = add(a, b);
    return plus[0] == constant1_term;
}
bool isContain(Poly& poly, Poly& subPoly)
{
    for (auto& term : subPoly)
        if (find(poly.begin(), poly.end(), term) == poly.end())
            return false;
    return true;
}
bool isReplace(Poly poly)
{
    sort(poly.begin(), poly.end(), MySort);   
    PolyTerm variable;
    for (auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;
    if (variable.count() <= 2)
        return false;
    if (poly.size() <= 1)
        return false;
    if (poly.size() == 2 && poly[1][loc_1] == 1)
        return false;
    return true;

}

int Judge_Poly(Poly poly)
{
    if (poly.size() == 0)
        return -3; 
    else if (poly.size() == 1 && poly[0].count() == 1)
    {
        if (poly[0][loc_1] == 1)
            return -2;  
        for (int i = 0; i < VARIABLE_COUNT; i++)
            if (poly[0][i] == 1)
                return i; 
    }
    return -1;
}

vector<Poly> zeroizePolynomials(vector<Poly> s) 
{
    vector<Poly> zeroized_polys;

    while (true)
    {
        map<Poly, int, PolyCompare> count;
        for (auto& poly : s)
        {
            sort(poly.begin(), poly.end(), MySort);
            if (poly[0][loc_1] == 0)
            {
                count[poly]++;
            }
        }

        Poly max_poly;
        int max_count = 0;
        for (auto& pair : count)
            if (pair.second > max_count)
            {
                max_count = pair.second;
                max_poly = pair.first;
            }
        //printPoly(max_poly);
        zeroized_polys.push_back(max_poly);

        vector<Poly> new_s;
        for (auto& poly : s)
        {
            if (isContain(poly, max_poly))
                poly = add(poly, max_poly);

            if (poly.size() > 1)
                new_s.push_back(poly);
            else if (poly.size() == 1 && poly[0][loc_1] == 0)
                new_s.push_back(poly);
        }
        if (new_s.size() == 0)
            break;
        s = new_s;
    }
    return zeroized_polys;
}
int zeroize(Poly poly, vector<Poly>& zero)
{
    for (auto& t : zero)
    {
        if (isContain(poly, t))
        {
            poly = add(poly, t);
            if (poly.size() == 0)
                return -3;
            else if (poly.size() == 1 && poly[0][loc_1] == 1)
                return -2;
        }
    }
}

void DATF(Poly poly, Poly& a, Poly& b)
{
    for (auto term : poly)
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
void ProcessingDATF1(int r, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B)
{
    int i;
    vector<Poly> result;
    for (i = 0; i < 320; i++)
    {
        DATF(s[i], A[r * 320 + i], B[r * 320 + i]);
        if (B[r * 320 + i].empty() == false)
            result.push_back(B[r * 320 + i]);
    }


    vector<Poly> zeroized_polys = zeroizePolynomials(result);

    cout << "Zeroized polys: " << endl;
    for (auto& poly : zeroized_polys)
        printPoly(poly);

    int flag1, flag2;
    Poly poly;
    PolyTerm term;

    for (i = 0; i < 320; i++)
    {
        if (isReplace(A[r * 320 + i]))
        {
            poly.clear();
            term.reset();
            term.set(loc_1 + 1 + 320 * r + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 320 + i];

        flag2 = Judge_Poly(B[r * 320 + i]);
        if (flag2 >= -1)
            flag2 = zeroize(B[r * 320 + i], zeroized_polys);
        if (flag2 == -2)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s[i] = poly;
    }
}
void ProcessingDATF2(int r, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B)
{
    int flag;
    Poly poly;
    PolyTerm term;

    map<Poly, int, PolyCompare> polyToVar;

    for (int i = 0; i < 320; i++)
    {
        DATF(s[i], A[r * 320 + i], B[r * 320 + i]);

        if (isReplace(A[r * 320 + i]))
        {
            poly.clear();
            term.reset();
            term.set(loc_1 + 1 + 320 * r + i);
            poly.push_back(term);
        }
        else
            poly = A[r * 320 + i];


        term.reset();
        term.set(loc_x);
        if (isReplace(B[r * 320 + i]))
        {
            auto it = polyToVar.find(B[r * 320 + i]);
            if (it != polyToVar.end())
                term.set(polyToVar[B[r * 320 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto& pair : polyToVar)
                {
                    if (isInverse(pair.first, B[r * 320 + i]))
                    {
                        term.set(polyToVar[pair.first]);
                        poly.push_back(term);
                        term.reset();
                        term.set(loc_x);
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVar[B[r * 320 + i]] = loc_1 + 1 + 320 * round + 320 * r + i;
                    term.set(loc_1 + 1 + 320 * round + 320 * r + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto& t : B[r * 320 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s[i] = poly;
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
vector<Poly> calculateANF(vector<int>& delta, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B, int flag_L)
{
    int i, j, k;
    bitset<128> IV(0);
    for (i = 0; i < delta.size(); i++)
        IV.set(delta[i]);

    Poly poly;
    PolyTerm term;
    term.set(loc_1);
    poly.push_back(term);

    for (i = 0; i < 64; i++)
    {
        if (i == 0 || i == 9 || i == 20 || i == 21 || i == 29 || i == 30)
            s[i] = poly;
    }


    for (i = 0; i < 128; i++)
    {
        poly.clear();
        term.reset();
        term.set(i);
        poly.push_back(term);
        s[64 + i] = poly;
    }


    for (i = 0; i < 128; i++)
    {
        poly.clear();
        term.reset();
        if (IV[i] == 1)
        {
            term.set(loc_x);
        }
        else
        {
            term.set(i + 128);
        }
        poly.push_back(term);
        s[192 + i] = poly;
    }

    int constant[96] = { 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1 };
    int offsets[5][2] = { 19,28,61,39,1,6,10,17,7,41 };


    cout << "Compute the  0  round" << endl;
    // pc
    for (j = 0; j < 8; j = j + 1)
    {
        if (constant[j] == 1)
            s[184 + j] = add_one(s[184 + j]);
    }
    // ps
    //  x0 ^= x4; x4 ^= x3; x2 ^= x1;
    for (j = 0; j < 64; j++)
    {
        s[j] = add(s[j], s[64 * 4 + j]);
        s[64 * 4 + j] = add(s[64 * 4 + j], s[64 * 3 + j]);
        s[64 * 2 + j] = add(s[64 * 2 + j], s[64 * 1 + j]);
    }

    for (i = 1; i < round; i++)
    {
        vector<Poly> t0, t1, t2, t3, t4;
        //t0 = x0; t1 = x1; t2 = x2; t3 = x3; t4 = x4;
        //t0 = ~t0; t1 = ~t1; t2 = ~t2; t3 = ~t3; t4 = ~t4;
        //t0 &= x1; t1 &= x2; t2 &= x3; t3 &= x4; t4 &= x0;
        for (j = 0; j < 64; j++)
        {
            t0.push_back(multiply(add_one(s[j]), s[64 + j]));
            t1.push_back(multiply(add_one(s[64 + j]), s[64 * 2 + j]));
            t2.push_back(multiply(add_one(s[64 * 2 + j]), s[64 * 3 + j]));
            t3.push_back(multiply(add_one(s[64 * 3 + j]), s[64 * 4 + j]));
            t4.push_back(multiply(add_one(s[64 * 4 + j]), s[j]));
        }
        //x0 ^= t1; x1 ^= t2; x2 ^= t3; x3 ^= t4; x4 ^= t0;
        for (j = 0; j < 64; j++)
        {
            s[64 * 0 + j] = add(s[64 * 0 + j], t1[j]);
            s[64 * 1 + j] = add(s[64 * 1 + j], t2[j]);
            s[64 * 2 + j] = add(s[64 * 2 + j], t3[j]);
            s[64 * 3 + j] = add(s[64 * 3 + j], t4[j]);
            s[64 * 4 + j] = add(s[64 * 4 + j], t0[j]);
        }
        //x1 ^= x0; x0 ^= x4; x3 ^= x2; x2 = ~x2;
        for (j = 0; j < 64; j++)
        {
            s[64 + j] = add(s[j], s[64 + j]);
            s[j] = add(s[64 * 4 + j], s[j]);
            s[64 * 3 + j] = add(s[64 * 3 + j], s[64 * 2 + j]);
            s[64 * 2 + j] = add_one(s[64 * 2 + j]);
        }

        //cout << "pl" << endl;
        for (j = 0; j < 5; j++)
        {
            vector<Poly> t;
            for (k = 0; k < offsets[j][0]; k = k + 1)
                t.push_back(s[64 * j + k + 64 - offsets[j][0]]);
            for (k = offsets[j][0]; k < 64; k = k + 1)
                t.push_back(s[64 * j + k - offsets[j][0]]);

            for (k = 0; k < offsets[j][1]; k = k + 1)
                t[k] = add(t[k], s[64 * j + k + 64 - offsets[j][1]]);
            for (k = offsets[j][1]; k < 64; k = k + 1)
                t[k] = add(t[k], s[64 * j + k - offsets[j][1]]);

            for (k = 0; k < 64; k = k + 1)
                s[64 * j + k] = add(s[64 * j + k], t[k]);
        }

        cout << "Compute the  " << i << "  round" << endl;

        // pc
       // cout << "pc" << endl;
        for (j = 0; j < 8; j = j + 1)
        {
            if (constant[8 * i + j] == 1)
                s[184 + j] = add_one(s[184 + j]);
        }

        // ps
        //  x0 ^= x4; x4 ^= x3; x2 ^= x1;
        //cout << "ps" << endl;
        for (j = 0; j < 64; j++)
        {
            s[j] = add(s[j], s[64 * 4 + j]);
            s[64 * 4 + j] = add(s[64 * 4 + j], s[64 * 3 + j]);
            s[64 * 2 + j] = add(s[64 * 2 + j], s[64 * 1 + j]);
        }


        if (i - 1 < N0)
            ProcessingDATF1(i - 1, s, A, B);
        else
            ProcessingDATF2(i - 1, s, A, B);

    }
    vector<Poly> t0, t1, t2, t3, t4;
    //t0 = x0; t1 = x1; t2 = x2; t3 = x3; t4 = x4;
    //t0 = ~t0; t1 = ~t1; t2 = ~t2; t3 = ~t3; t4 = ~t4;
    //t0 &= x1; t1 &= x2; t2 &= x3; t3 &= x4; t4 &= x0;
    for (j = 0; j < 64; j++)
    {
        t0.push_back(multiply(add_one(s[j]), s[64 + j]));
        t1.push_back(multiply(add_one(s[64 + j]), s[64 * 2 + j]));
        t2.push_back(multiply(add_one(s[64 * 2 + j]), s[64 * 3 + j]));
        t3.push_back(multiply(add_one(s[64 * 3 + j]), s[64 * 4 + j]));
        t4.push_back(multiply(add_one(s[64 * 4 + j]), s[j]));
    }
    //x0 ^= t1; x1 ^= t2; x2 ^= t3; x3 ^= t4; x4 ^= t0;
    for (j = 0; j < 64; j++)
    {
        s[64 * 0 + j] = add(s[64 * 0 + j], t1[j]);
        s[64 * 1 + j] = add(s[64 * 1 + j], t2[j]);
        s[64 * 2 + j] = add(s[64 * 2 + j], t3[j]);
        s[64 * 3 + j] = add(s[64 * 3 + j], t4[j]);
        s[64 * 4 + j] = add(s[64 * 4 + j], t0[j]);
    }
    //x1 ^= x0; x0 ^= x4; x3 ^= x2; x2 = ~x2;
    for (j = 0; j < 64; j++)
    {
        s[64 + j] = add(s[j], s[64 + j]);
        s[j] = add(s[64 * 4 + j], s[j]);
        s[64 * 3 + j] = add(s[64 * 3 + j], s[64 * 2 + j]);
        s[64 * 2 + j] = add_one(s[64 * 2 + j]);
    }
    
    if (flag_L == 1)
    {
        for (j = 0; j < 5; j++)
        {
            vector<Poly> t;
            for (k = 0; k < offsets[j][0]; k = k + 1)
                t.push_back(s[64 * j + k + 64 - offsets[j][0]]);
            for (k = offsets[j][0]; k < 64; k = k + 1)
                t.push_back(s[64 * j + k - offsets[j][0]]);

            for (k = 0; k < offsets[j][1]; k = k + 1)
                t[k] = add(t[k], s[64 * j + k + 64 - offsets[j][1]]);
            for (k = offsets[j][1]; k < 64; k = k + 1)
                t[k] = add(t[k], s[64 * j + k - offsets[j][1]]);

            for (k = 0; k < 64; k = k + 1)
                s[64 * j + k] = add(s[64 * j + k], t[k]);
        }
    }
    

    vector<Poly> difference;
    for (i = 0; i < 320; i++)
        difference.push_back(calculatedifference(s[i]));
    return difference;
}



//   Part 3: Backward Substitution Expression

int find_isolate(Poly poly)
{
    int flag;
    for (int i = round - 1; i >= 0; i--)
    {

        for (int j = 0; j < 320; j++)
        {

            flag = 0;
            for (auto term : poly)
            {
                if (term[loc_1 + 1 + 320 * round + 320 * i + j] == 1)
                {
                    flag = flag + term.count();
                    if (flag > 1)
                        break;
                }
            }
            if (flag == 1)
                return loc_1 + 1 + 320 * round + 320 * i + j;


            flag = 0;
            for (auto term : poly)
            {
                if (term[loc_1 + 1 + 320 * i + j] == 1)
                {
                    flag = flag + term.count();
                    if (flag > 1)
                        break;
                }
            }
            if (flag == 1)
                return loc_1 + 1 + 320 * i + j;


        }
    }
    return -1;
}
Poly pushback(Poly difference1, vector<Poly>& A, vector<Poly>& B)
{
    int isolate;
    Poly poly, condition;

    isolate = find_isolate(difference1);
    while (isolate >= 0)
    {
        if (isolate < loc_1 + 1 + 320 * round)
            condition = A[isolate - (loc_1 + 1)];
        else
            condition = B[isolate - (loc_1 + 1) - 320 * round];

        poly.clear();

        for (auto& term : difference1)
        {
            if (term[isolate] != 1)
                poly.push_back(term);
        }
        difference1 = add(poly, condition);
        isolate = find_isolate(difference1);
    }
    return difference1;
}

//   Part 4: Evaluating the Bias
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
        for (auto& term : Separate[i])
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
                    if (variables[k] < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[variables[k] - loc_1 - 1 - 320 * round]);
                }
                else
                {
                    if (variables[k] < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[variables[k] - loc_1 - 1 - 320 * round]);
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
                    if (loc < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[loc - loc_1 - 1 - 320 * round]);
                    flag = 1;
                    break;
                }
                else if (b1 == 0)
                {
                    poly = f0;
                    if (loc < loc_1 + 1 + 320 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[loc - loc_1 - 1 - 320 * round]);
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
                    if (loc < loc_1 + 1 + 320 * round)
                        p_0 = 0.5;
                    else
                        p_0 = 0.5 + B_bias[loc - loc_1 - 1 - 320 * round];

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


void calculateBbias(vector<Poly> B, vector<double>& B_bias)
{
    for (int i = 0; i < 320 * round; i++)
    {
        if (B[i].size() == 0)
            B_bias[i] = 0.5;
        else if (B[i].size() == 1 && B[i][0][loc_1] == 1)
            B_bias[i] = -0.5;
        else
            B_bias[i] = calculatebias_v2(B[i], B_bias);
    }
}






void CDL_distinguishers_5round()
{
    cout << "Calculate the bias of the CDL distinguisher for 5-round Ascon, excluding linear layers." << endl;
    vector<int> delta = { 0,64 };
    vector<Poly> s(320);
    vector<Poly> A(320 * round), B(320 * round);
    vector<Poly> difference = calculateANF(delta, s, A, B, 0);
    cout << "The DATF has been calculated." << endl << endl;

    printDATF_file(A, B);

    vector<double> B_bias(320 * round);
    calculateBbias(B, B_bias);



    clock_t start, end;
    for (int i = 0; i < 64; i++)
    {
        Poly difference2 = pushback(difference[i], A, B);
        start = clock();
        double bias = calculatebias_v2(difference2, B_bias);
        end = clock();

        cout << i << "   bias: " << bias ;
        cout << "     time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    }


}


void CDL_distinguishers_6round()
{
    cout << "Calculate the bias of the CDL distinguisher for 5-round Ascon, including linear layers." << endl;
    vector<int> delta = { 0,64 };
    vector<Poly> s(320);
    vector<Poly> A(320 * round), B(320 * round);
    vector<Poly> difference = calculateANF(delta, s, A, B, 1);
    cout << "The DATF has been calculated." << endl << endl;
    printDATF_file(A, B);


    vector<double> B_bias(320 * round);
    calculateBbias(B, B_bias);

    clock_t start, end;
    for (int i = 256; i < 320; i++)
    {
        Poly difference2 = pushback(difference[i], A, B);
        start = clock();
        double bias = calculatebias_v2(difference2, B_bias);
        end = clock();

        cout << i << "   bias: " << bias;
        cout << "     time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    }


}


int main()
{

    //  CDL distinguishers for 5-round Ascon
    CDL_distinguishers_5round();

    cout << endl << "------------------------------------------------------------------" << endl << endl;

    //  CDL distinguishers for 6-round Ascon
    CDL_distinguishers_6round();


    return 0;
}



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



#define round 4
const int VARIABLE_COUNT = 128 + 256 + 2 + 2 * 384 * round;      // x + const 1 + ATF
const int loc_x = 384;
const int loc_1 = 384 + 1;
using PolyTerm = bitset<VARIABLE_COUNT>;
using Poly = vector<PolyTerm>;


struct XoodooState {
    Poly state[4][3][32];
};





bool MySort(PolyTerm a, PolyTerm b)
{
    for (int i = 0; i < 384; i++)
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
            if (term[i] == 1)   cout << "k" << i;
        for (i = 0; i < 256; i++)
            if (term[i + 128] == 1)   cout << "v" << i;
        for (i = 0; i < 384 * round; i++)
            if (term[i + loc_1 + 1] == 1)   cout << "a" << i;
        for (i = 0; i < 384 * round; i++)
            if (term[i + loc_1 + 1 + 384 * round] == 1)   cout << "b" << i;
        if (term[loc_x] == 1)   cout << "x";
        if (term[loc_1] == 1)   cout << "1";
        cout << "+";
    }
    cout << endl;
}
void printDATF_file(int r, vector<Poly>& A, vector<Poly>& B)
{
    int i, loc;
    ofstream outfile;
    outfile.open("DATF.txt");

    for (loc = 0; loc < 384 * r; loc++)
    {
        sort(A[loc].begin(), A[loc].end(), MySort);
        outfile << "a" << loc << ":";
        for (auto& term : A[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "k" << i;
            for (i = 0; i < 256; i++)
                if (term[i + 128] == 1)   outfile << "v" << i;
            for (i = 0; i < 384 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 384 * round; i++)
                if (term[i + loc_1 + 1 + 384 * round] == 1)   outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < 384 * r; loc++)
    {
        sort(B[loc].begin(), B[loc].end(), MySort);
        outfile << "b" << loc << ":";
        for (auto& term : B[loc])
        {
            for (i = 0; i < 128; i++)
                if (term[i] == 1)   outfile << "k" << i;
            for (i = 0; i < 256; i++)
                if (term[i + 128] == 1)   outfile << "v" << i;
            for (i = 0; i < 384 * round; i++)
                if (term[i + loc_1 + 1] == 1)   outfile << "a" << i;
            for (i = 0; i < 384 * round; i++)
                if (term[i + loc_1 + 1 + 384 * round] == 1)   outfile << "b" << i;
            if (term[loc_x] == 1)   outfile << "x";
            if (term[loc_1] == 1)   outfile << "1";
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    outfile.close();
}

void Rotate(Poly A[4][32], int index1, int index2, Poly B[4][32])
{
    for (int z = 0; z < 32; z++)
        for (int x = 0; x < 4; x++)
            B[x][z] = A[(x + 4 - index1) % 4][(z + 32 - index2) % 32];
}

void theta(XoodooState& s)
{
    Poly P[4][32], E[4][32];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 32; j++)
        {
            vector<Poly> T = { s.state[i][0][j],s.state[i][1][j],s.state[i][2][j] };
            P[i][j] = addmore(T);
        }


    Poly P15[4][32], P114[4][32];

    Rotate(P, 1, 5, P15);
    Rotate(P, 1, 14, P114);


    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 32; j++)
            E[i][j] = add(P15[i][j], P114[i][j]);


    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 32; k++)
                s.state[i][j][k] = add(s.state[i][j][k], E[i][k]);
}


void rho_west(XoodooState& s)
{
    Poly tA1[4][32], tA2[4][32];
    Poly tB1[4][32], tB2[4][32];
    for (int i = 0; i < 4; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            tA1[i][k] = s.state[i][1][k];
            tA2[i][k] = s.state[i][2][k];
        }
    }

    Rotate(tA1, 1, 0, tB1);
    Rotate(tA2, 0, 11, tB2);

    for (int i = 0; i < 4; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            s.state[i][1][k] = tB1[i][k];
            s.state[i][2][k] = tB2[i][k];
        }
    }
}

void iota(XoodooState& s, int C)
{
    for (int i = 0; i < 32; i++)
        if (C >> (31 - i) & 0x1)
            s.state[0][0][i] = add_one(s.state[0][0][i]);
}

void chi(XoodooState& s)
{
    for (int row = 0; row < 4; row++)
    {
        for (int k = 0; k < 32; k++)
        {
            Poly a = s.state[row][0][k];
            Poly b = s.state[row][1][k];
            Poly c = s.state[row][2][k];
            s.state[row][0][k] = add(a, multiply(add_one(b), c));
            s.state[row][1][k] = add(b, multiply(add_one(c), a));
            s.state[row][2][k] = add(c, multiply(add_one(a), b));
        }

    }
}


void rho_east(XoodooState& s)
{
    Poly tA1[4][32], tA2[4][32];
    Poly tB1[4][32], tB2[4][32];
    for (int i = 0; i < 4; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            tA1[i][k] = s.state[i][1][k];
            tA2[i][k] = s.state[i][2][k];
        }
    }

    Rotate(tA1, 0, 1, tB1);
    Rotate(tA2, 2, 8, tB2);


    for (int i = 0; i < 4; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            s.state[i][1][k] = tB1[i][k];
            s.state[i][2][k] = tB2[i][k];
        }
    }
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

    return true;

}
bool isInverse(const Poly& a, const Poly& b)
{
    PolyTerm constant1_term;
    constant1_term.set(loc_1);
    Poly plus = add(a, b);
    return plus[0] == constant1_term;
}

bool isInVector(const std::vector<int>& vec, int a) {
    return std::find(vec.begin(), vec.end(), a) != vec.end();
}

void ProcessingDATF(int r, XoodooState& s, vector<Poly>& A, vector<Poly>& B)
{
    int flag;
    Poly poly;
    PolyTerm term;
    map<Poly, int, PolyCompare> polyToVarB;

    for (int i = 0; i < 384; i++)
    {
        DATF(s.state[(i % 128) / 32][i / 128][i % 32], A[r * 384 + i], B[r * 384 + i]);

        poly.clear();
        if (isReplace(A[r * 384 + i]))
        {
            term.reset();
            term.set(loc_1 + 1 + 384 * r + i);
            poly.push_back(term);
        }
        else
        {
            for (auto t : A[r * 384 + i])
            {
                poly.push_back(t);
            }
        }


        if (isReplace(B[r * 384 + i]))
        {
            term.reset();
            term.set(loc_x);
            auto it = polyToVarB.find(B[r * 384 + i]);
            if (it != polyToVarB.end())
                term.set(polyToVarB[B[r * 384 + i]]);
            else
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[r * 384 + i]))
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
                    polyToVarB[B[r * 384 + i]] = loc_1 + 1 + 384 * round + 384 * r + i;
                    term.set(loc_1 + 1 + 384 * round + 384 * r + i);
                }
            }
            poly.push_back(term);
        }
        else
        {
            for (auto t : B[r * 384 + i])
            {
                if (t[loc_1] == 1)
                    t.reset(loc_1);
                t.set(loc_x);
                poly.push_back(t);
            }
        }
        s.state[(i % 128) / 32][i / 128][i % 32] = poly;
    }

}



vector<int> ProcessingDATFV2(XoodooState& s)
{
    vector<Poly> B(384);
    Poly poly;
    PolyTerm term;
    map<Poly, int, PolyCompare> polyToVarB;

    vector<int> subset_IV;
    for (int i = 0; i < 384; i++)
    {
        B[i] = calculatedifference(s.state[(i % 128) / 32][i / 128][i % 32]);

        if (B[i].size() > 0 && B[i][0][loc_1] != 1)
        {
            auto it = polyToVarB.find(B[i]);
            if (it == polyToVarB.end())
            {
                bool hasInverse = false;
                for (auto pair : polyToVarB)
                {
                    if (isInverse(pair.first, B[i]))
                    {
                        hasInverse = true;
                        break;
                    }
                }
                if (!hasInverse)
                {
                    polyToVarB[B[i]] = i;
                    subset_IV.push_back(i);
                }

            }
        }
    }

    return subset_IV;

}



vector<int> calculate_subset(vector<int> delta, XoodooState& s)
{
    unsigned int C[12] = { 0x58, 0x38, 0x3C0, 0xD0, 0x120, 0x14, 0x60, 0x2C, 0x380, 0xF0, 0x1A0, 0x12 };
    int i, j;

    bitset<384> IV(0);
    for (int value : delta)
        IV.set(value);

    Poly poly;
    PolyTerm term;
    for (i = 0; i < 384; i++)
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
        s.state[(i % 128) / 32][i / 128][i % 32] = poly;
    }

    theta(s);
    rho_west(s);
    iota(s, C[0]);
    chi(s);
    rho_east(s);
    theta(s);
    rho_west(s);
    iota(s, C[1]);



    return ProcessingDATFV2(s);
}


Poly calculateANF(int r, vector<int> subset, int loc, vector<int> delta, vector<int> lambda, XoodooState& s, vector<Poly>& A, vector<Poly>& B)
{
    unsigned int C[12] = { 0x58, 0x38, 0x3C0, 0xD0, 0x120, 0x14, 0x60, 0x2C, 0x380, 0xF0, 0x1A0, 0x12 };
    int i, j;

    bitset<384> IV(0);
    for (int value : delta)
        IV.set(value);

    Poly poly;
    PolyTerm term;
    for (i = 0; i < 384; i++)
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
        s.state[(i % 128) / 32][i / 128][i % 32] = poly;
    }

    theta(s);
    rho_west(s);
    iota(s, C[0]);
    for (int rrr = 1; rrr < r; rrr++)
    {
        // cout << " --------------- " << rrr << " --------------- " << endl;


        chi(s);
        rho_east(s);
        theta(s);
        rho_west(s);
        iota(s, C[rrr]);

        if (rrr < r)
        {
            ProcessingDATF(rrr - 1, s, A, B);
        }


        if (rrr == 1)
        {
            for (i = 0; i < 384; i++)
            {
                PolyTerm vvv;
                for (PolyTerm t : s.state[(i % 128) / 32][i / 128][i % 32])
                    vvv |= t;
                for (int index = 0; index < subset.size(); index++)
                {
                    if (vvv[subset[index] + loc_1 + 1 + 384 * round] == 1)
                    {
                        poly.clear();
                        for (PolyTerm t : s.state[(i % 128) / 32][i / 128][i % 32])
                        {
                            if (t[subset[index] + loc_1 + 1 + 384 * round] != 1)
                            {
                                poly.push_back(t);
                            }
                            else
                            {
                                if ((loc >> index) & 1)
                                {
                                    term.reset();
                                    term.set(loc_x);
                                    poly.push_back(term);
                                }
                            }
                        }

                        s.state[(i % 128) / 32][i / 128][i % 32] = poly;
                        break;
                    }
                }
            }
        }

    }
    chi(s);
    rho_east(s);

    vector<Poly> T;
    bitset<192> lamb(0);
    for (i = 0; i < lambda.size(); i++)
        lamb.set(lambda[i]);

    for (i = 0; i < 192; i++)
        if (lamb[i])  T.push_back(calculatedifference(s.state[(i % 128) / 32][i / 128][i % 32]));

    return addmore(T);

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
                    if (variables[k] < loc_1 + 1 + 384 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[variables[k] - loc_1 - 1 - 384 * round]);
                }
                else
                {
                    if (variables[k] < loc_1 + 1 + 384 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[variables[k] - loc_1 - 1 - 384 * round]);
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
                    if (loc < loc_1 + 1 + 384 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 - B_bias[loc - loc_1 - 1 - 384 * round]);
                    flag = 1;
                    break;
                }
                else if (b1 == 0)
                {
                    poly = f0;
                    if (loc < loc_1 + 1 + 384 * round)
                        prob = prob * 0.5;
                    else
                        prob = prob * (0.5 + B_bias[loc - loc_1 - 1 - 384 * round]);
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
                    if (loc < loc_1 + 1 + 384 * round)
                        p_0 = 0.5;
                    else
                        p_0 = 0.5 + B_bias[loc - loc_1 - 1 - 384 * round];

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
    //  cout << "----------------  Calculate B bias  ----------------" << endl;
    for (int i = 0; i < 384 * r; i++)
    {
        if (B[i].size() == 0)
            B_bias[i] = 0.5;
        else if (B[i].size() == 1 && B[i][0][loc_1] == 1)
            B_bias[i] = -0.5;
        else
            B_bias[i] = calculatebias_v2(B[i], B_bias);

        //cout << i << ": " << B_bias[i] << endl;
    }
}


int main()
{


     // int r = 4;       // 0.0012207
    //  vector<int> delta = { 128,256 };
     // vector<int> lambda = { 128+15 };


      int r = 4;         // -0.0244141
      vector<int> delta = { 0,256 };
      vector<int> lambda = { 0 };




    XoodooState s;
    vector<int> subset = calculate_subset(delta, s);

    cout << "subset:  ";
    for (int num : subset)
    {
        cout << num << ", ";
    }
    cout << endl;


    double bias = 0;

#pragma omp parallel for
    for (int loc = 0; loc < (1 << (subset.size())); loc++)
    {
        XoodooState s;
        vector<Poly> A(384 * r), B(384 * r);
        Poly difference = calculateANF(r, subset, loc, delta, lambda, s, A, B);
        //printPoly(difference);
      //  printDATF_file(r, A, B);
        vector<double> B_bias(384 * r);
        calculateBbias(r - 1, B, B_bias);

        double b_temp = calculatebias_v2(difference, B_bias);

#pragma omp critical
        {
            bias += b_temp;
            //printPoly(difference1);
            cout << loc << ": " << b_temp << endl;
        }
    }


    cout << "bias: " << bias / (1 << (subset.size())) << endl;

    system("pause");
    return 0;
}

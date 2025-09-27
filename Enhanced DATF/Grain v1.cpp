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
#define round 126
using namespace std;


const int N0 = 51;
const int VARIABLE_COUNT = 80 + 64 + 1 + 1 + 4 * round;       // key(80) + IV(64) + x + const 1 + ATF
const int loc_x = 144;
const int loc_1 = 145;
using PolyTerm = bitset<VARIABLE_COUNT>;
using Poly = vector<PolyTerm>;







//   Part 1: Output by Format
bool MySort(PolyTerm a, PolyTerm b)
{
    int i;
    for (i = 80; i < 80 + 64; i++)
        if (a[i] > b[i])
            return true;
        else
            return false;
    for (i = 0; i < 80; i++)
        if (a[i] > b[i])
            return true;
        else
            return false;
    for (i = loc_1 + 1; i < VARIABLE_COUNT; i++)
        if (a[i] > b[i])
            return true;
        else
            return false;
    if (a[loc_x] > b[loc_x])
        return true;

    return false;
}
void printPoly(Poly poly)
{
    sort(poly.begin(), poly.end(), MySort);
    int i;
    for (auto& term : poly)
    {
        for (i = 0; i < 80; i++)
        {
            if (term[i] == 1)
                cout << "k" << i;
        }
        for (i = 0; i < 64; i++)
        {
            if (term[i + 80] == 1)
                cout << "v" << i;
        }
        if (term[loc_1] == 1)
            cout << "1";
        for (i = 0; i < round; i++)
        {
            if (term[i + 146] == 1)
                cout << "a" << i;
        }
        for (i = 0; i < round; i++)
        {
            if (term[i + 146 + round] == 1)
                cout << "b" << i;
        }
        for (i = 0; i < round; i++)
        {
            if (term[i + 146 + 2 * round] == 1)
                cout << "c" << i;
        }
        for (i = 0; i < round; i++)
        {
            if (term[i + 146 + 3 * round] == 1)
                cout << "d" << i;
        }
        if (term[loc_x] == 1)
            cout << "x";
        cout << "+";
    }
    cout << endl;
}
void printDATF_file(vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D)
{
    int i, loc;
    ofstream outfile;
    outfile.open("DATF.txt");

    for (loc = 0; loc < round; loc++)
    {
        sort(A[loc].begin(), A[loc].end(), MySort);
        outfile << "a" << loc << ":";
        for (auto& term : A[loc])
        {
            for (i = 0; i < 64; i++)
            {
                if (term[i + 80] == 1)
                    outfile << "v" << i;
            }
            for (i = 0; i < 80; i++)
            {
                if (term[i] == 1)
                    outfile << "k" << i;
            }
            if (term[145] == 1)
                outfile << "1";
            for (i = 0; i < round; i++)
            {
                if (term[i + 146] == 1)
                    outfile << "a" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + round] == 1)
                    outfile << "b" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 2 * round] == 1)
                    outfile << "c" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 3 * round] == 1)
                    outfile << "d" << i;
            }
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < round; loc++)
    {
        sort(B[loc].begin(), B[loc].end(), MySort);
        outfile << "b" << loc << ":";
        for (auto& term : B[loc])
        {
            for (i = 0; i < 64; i++)
            {
                if (term[i + 80] == 1)
                    outfile << "v" << i;
            }
            for (i = 0; i < 80; i++)
            {
                if (term[i] == 1)
                    outfile << "k" << i;
            }
            if (term[145] == 1)
                outfile << "1";
            for (i = 0; i < round; i++)
            {
                if (term[i + 146] == 1)
                    outfile << "a" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + round] == 1)
                    outfile << "b" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 2 * round] == 1)
                    outfile << "c" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 3 * round] == 1)
                    outfile << "d" << i;
            }
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < round; loc++)
    {
        outfile << "c" << loc << ":";
        for (auto& term : C[loc])
        {
            for (i = 0; i < 64; i++)
            {
                if (term[i + 80] == 1)
                    outfile << "v" << i;
            }
            for (i = 0; i < 80; i++)
            {
                if (term[i] == 1)
                    outfile << "k" << i;
            }
            if (term[145] == 1)
                outfile << "1";
            for (i = 0; i < round; i++)
            {
                if (term[i + 146] == 1)
                    outfile << "a" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + round] == 1)
                    outfile << "b" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 2 * round] == 1)
                    outfile << "c" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 3 * round] == 1)
                    outfile << "d" << i;
            }
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    for (loc = 0; loc < round; loc++)
    {
        outfile << "d" << loc << ":";
        for (auto& term : D[loc])
        {
            for (i = 0; i < 64; i++)
            {
                if (term[i + 80] == 1)
                    outfile << "v" << i;
            }
            for (i = 0; i < 80; i++)
            {
                if (term[i] == 1)
                    outfile << "k" << i;
            }
            if (term[145] == 1)
                outfile << "1";
            for (i = 0; i < round; i++)
            {
                if (term[i + 146] == 1)
                    outfile << "a" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + round] == 1)
                    outfile << "b" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 2 * round] == 1)
                    outfile << "c" << i;
            }
            for (i = 0; i < round; i++)
            {
                if (term[i + 146 + 3 * round] == 1)
                    outfile << "d" << i;
            }
            outfile << "+";
        }
        outfile << endl;
    }
    outfile << endl;

    outfile.close();
}
bool isReplace(Poly poly)
{
    sort(poly.begin(), poly.end(), MySort);
    if (poly.size() <= 1)
        return false;
    if (poly.size() == 2 && poly[1][loc_1] == 1)
        return false;
    return true;
}
//   Part 2: Calculating the DATF
Poly add(Poly a, Poly b)
{
    Poly result;
    result.insert(result.end(), a.begin(), a.end());
    result.insert(result.end(), b.begin(), b.end());

    unordered_map<PolyTerm, int> occurrences;
    for (auto& term : result)
        occurrences[term]++;

    result.clear();
    for (auto& pair : occurrences)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);

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

    return result;
}
Poly multiply(Poly a, Poly b)
{
    Poly result;
    for (auto& termA : a)
    {
        for (auto& termB : b)
        {
            PolyTerm newTerm = termA | termB;
            result.push_back(newTerm);
        }
    }

    unordered_map<PolyTerm, int> occurrences;
    for (auto& term : result)
    {
        if (term[loc_1] == 1 && term.count() > 1)
            term[loc_1] = 0;
        occurrences[term]++;
    }

    result.clear();
    for (auto& pair : occurrences)
        if (pair.second % 2 == 1)
            result.push_back(pair.first);

    return result;
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
int Judge_equality(Poly poly1, Poly poly2)
{
    int i;
    int c1 = poly1.size();
    int c2 = poly2.size();
    if (c1 == 0 && c2 == 0)
        return 0;    //000
    else if (c1 == 0 && c2 == 1 && poly2[0][loc_1] == 1)
        return 1;    //001
    else if (c1 == 1 && c2 == 0 && poly1[0][loc_1] == 1)
        return 2;    //010
    else if (c1 == 1 && c2 == 1 && poly1[0][loc_1] == 1 && poly2[0][loc_1] == 1)
        return 3;    //011

    Poly t = add(poly1, poly2);
    if (t.size() == 0)    // equal 100
        return 4;
    else if (t.size() == 1 && t[0][loc_1] == 1)
    {
        if (c1 - c2 == 1)  //110
            return 6;
        else if (c2 - c1 == 1)  //101
            return 5;
    }
    return 7;  //111
}
int Judge_difference(Poly difference)
{
    if (difference.size() == 0)
        return -1;
    if (difference.size() == 1 && difference[0].count() == 1)
    {
        for (int i = 0; i < VARIABLE_COUNT; i++)
            if (difference[0][i] == 1)
                return i;
    }
    return -2;
}
Poly calculateANF(vector<int> loc, vector<Poly>& b, vector<Poly>& s, vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D)
{
    // b: a+b*x;  s: c+d*x
    int i, j;

    bitset<64> IV(0);
    for (i = 0; i < loc.size(); i++)
        IV.set(loc[i]);

    Poly poly, constant1;
    PolyTerm term(0);
    term.set(loc_1);
    constant1.push_back(term);

    // initialization
    for (i = 0; i < 80; i++)
    {
        poly.clear();
        term.reset();
        term.set(i);
        poly.push_back(term);
        b.push_back(poly);
    }
    for (i = 0; i < 64; i++)
    {
        poly.clear();
        term.reset();
        term.set(i + 80);
        poly.push_back(term);
        if (IV[i] == 1)
        {
            term.reset();
            term.set(loc_x);
            poly.push_back(term);
        }
        s.push_back(poly);
    }
    for (i = 64; i < 80; i++)
        s.push_back(constant1);

    cout << "b: ";
    for (i = 0; i < 80; i++)
        printPoly(b[i]);

    cout << "s: ";
    for (i = 0; i < 80; i++)
        printPoly(s[i]);


    Poly t1, t2, z, f1;
    vector<Poly> T1, T2, Z, F1;

    for (i = 0; i < round; i++)
    {
        //cout << "Compute the  " << i << "  round" << endl;

        // b{1,2,4,10,31,43,56,63} + s[25]
        // s[3]s[25]s[46]+s[3]s[46]b[63]+s[25]s[46]b[63]
        // s[64](s[3] + s[46] + b[63] + s[3]s[46] + s[46]b[63])
        Z.clear();
        Z.push_back(b[1]);
        Z.push_back(b[2]);
        Z.push_back(b[4]);
        Z.push_back(b[10]);
        Z.push_back(b[31]);
        Z.push_back(b[43]);
        Z.push_back(b[56]);
        Z.push_back(b[63]);
        Z.push_back(s[25]);
        Z.push_back(multiply(multiply(s[3], s[25]), s[46]));
        Z.push_back(multiply(multiply(s[3], s[46]), b[63]));
        Z.push_back(multiply(multiply(s[25], s[46]), b[63]));
        F1.clear();
        F1.push_back(s[3]);
        F1.push_back(s[46]);
        F1.push_back(b[63]);
        F1.push_back(multiply(s[3], s[46]));
        F1.push_back(multiply(s[46], b[63]));
        f1 = addmore(F1);
        Z.push_back(multiply(s[64], f1));
        z = addmore(Z);


        //  s0 + b{ 62,60,52,45,37,33,28,21,14,9,0 }
        //  b33b28b21(1 + b15b9 + b52b45b37)
        //  b63b60(1 + b21b15 + b52b45b37)
        //  b37b33 + b15b9 + b60b52b45 + b63b45b28b9 + b60b52b37b33
        T2.clear();
        int ggg[11] = { 62,60,52,45,37,33,28,21,14,9,0 };
        T2.push_back(s[0]);
        for (j = 0; j < 11; j++)
            T2.push_back(b[ggg[j]]);
        F1.clear();
        F1.push_back(constant1);
        F1.push_back(multiply(b[15], b[9]));
        F1.push_back(multiply(multiply(b[52], b[45]), b[37]));
        f1 = addmore(F1);
        T2.push_back(multiply(multiply(multiply(b[33], f1), b[28]), b[21]));
        F1.clear();
        F1.push_back(constant1);
        F1.push_back(multiply(b[21], b[15]));
        F1.push_back(multiply(multiply(b[52], b[45]), b[37]));
        f1 = addmore(F1);
        T2.push_back(multiply(multiply(b[63], f1), b[60]));
        T2.push_back(multiply(b[37], b[33]));
        T2.push_back(multiply(b[15], b[9]));
        T2.push_back(multiply(multiply(b[60], b[52]), b[45]));
        T2.push_back(multiply(multiply(multiply(b[63], b[45]), b[28]), b[9]));
        T2.push_back(multiply(multiply(multiply(b[60], b[52]), b[37]), b[33]));
        t2 = addmore(T2);

        //t1 =  s62 + s51 + s38 + s23 + s13 + s0
        T1.clear();
        T1.push_back(s[62]);
        T1.push_back(s[51]);
        T1.push_back(s[38]);
        T1.push_back(s[23]);
        T1.push_back(s[13]);
        T1.push_back(s[0]);
        t1 = addmore(T1);


        b.erase(b.begin());
        s.erase(s.begin());
        if (i < N0)
        {
            Poly pb, ps;

            f1.clear();
            f1 = add(t2, z);
            DATF(f1, A[i], B[i]);
            f1.clear();
            f1 = add(t1, z);
            DATF(f1, C[i], D[i]);

            term.reset();
            term.set(loc_1 + 1 + i);
            pb.push_back(term);
            term.reset();
            term.set(loc_1 + 1 + 2 * round + i);
            ps.push_back(term);


            term.reset();
            switch (Judge_equality(B[i], D[i]))
            {
            case 1:
            case 5:
                term.set(loc_x);
                ps.push_back(term);
                break;
            case 2:
            case 6:
                term.set(loc_x);
                pb.push_back(term);
                break;
            case 3:
                term.set(loc_x);
                pb.push_back(term);
                ps.push_back(term);
                break;
            default:
                break;
            }



            b.push_back(pb);
            s.push_back(ps);
        }
        else
        {
            int flag;
            Poly pb, ps;

            f1.clear();
            f1 = add(t2, z);
            DATF(f1, A[i], B[i]);  // b:  a+bx
            f1.clear();
            f1 = add(t1, z);
            DATF(f1, C[i], D[i]);  // s:  c+dx

            term.reset();
            term.set(loc_1 + 1 + i);
            pb.push_back(term);
            term.reset();
            term.set(loc_1 + 1 + 2 * round + i);
            ps.push_back(term);


            if (isReplace(B[i]) && isReplace(D[i]))
            {


                term.reset();
                switch (Judge_equality(B[i], D[i]))
                {
                case 1:
                    term.set(loc_x);
                    ps.push_back(term);
                    break;
                case 2:
                    term.set(loc_x);
                    pb.push_back(term);
                    break;
                case 3:
                    term.set(loc_x);
                    pb.push_back(term);
                    ps.push_back(term);
                    break;
                case 4:    // 4 equal
                    flag = Judge_difference(B[i]);
                    if (flag >= 0 && flag != loc_1)
                        term.set(flag);
                    else
                        term.set(loc_1 + 1 + round + i);
                    term.set(loc_x);
                    pb.push_back(term);
                    ps.push_back(term);
                    break;
                case 5:    // 101
                    flag = Judge_difference(B[i]);
                    if (flag >= 0 && flag != loc_1)
                        term.set(flag);
                    else
                        term.set(loc_1 + 1 + round + i);
                    term.set(loc_x);
                    pb.push_back(term);
                    ps.push_back(term);
                    term.reset();
                    term.set(loc_x);
                    ps.push_back(term);
                    break;
                case 6:   //110
                    flag = Judge_difference(D[i]);
                    if (flag >= 0 && flag != loc_1)
                        term.set(flag);
                    else
                        term.set(loc_1 + 1 + 3 * round + i);
                    term.set(loc_x);
                    pb.push_back(term);
                    ps.push_back(term);
                    term.reset();
                    term.set(loc_x);
                    pb.push_back(term);
                    break;
                case 7:   //  7 无关
                    flag = Judge_difference(B[i]);
                    if (flag != -1)
                    {
                        if (flag >= 0 && flag != loc_1)
                            term.set(flag);
                        else
                            term.set(loc_1 + 1 + round + i);
                        term.set(loc_x);
                        pb.push_back(term);
                    }
                    term.reset();
                    flag = Judge_difference(D[i]);
                    if (flag != -1)
                    {
                        if (flag >= 0 && flag != loc_1)
                            term.set(flag);
                        else
                            term.set(loc_1 + 1 + 3 * round + i);
                        term.set(loc_x);
                        ps.push_back(term);
                    }
                    break;
                default:
                    break;
                }
            }
            else if (isReplace(B[i]))   //B替换，D不替换
            {
                for (auto t : D[i])
                {
                    if (t[loc_1] == 1)
                    {
                        t.set(loc_x);
                        t.reset(loc_1);
                    }
                    else
                        t.set(loc_x);
                    ps.push_back(t);
                }

                term.reset();
                term.set(loc_1 + 1 + round + i);
                term.set(loc_x);
                pb.push_back(term);


            }
            else if (isReplace(D[i]))   //B不替换，D替换
            {
                for (auto t : B[i])
                {
                    if (t[loc_1] == 1)
                    {
                        t.set(loc_x);
                        t.reset(loc_1);
                    }
                    else
                        t.set(loc_x);

                    pb.push_back(t);
                }
                term.reset();
                term.set(loc_1 + 1 + 3 * round + i);
                term.set(loc_x);
                ps.push_back(term);
            }
            else  //B不替换，D不替换
            {
                for (auto t : B[i])
                {
                    if (t[loc_1] == 1)
                    {
                        t.set(loc_x);
                        t.reset(loc_1);
                    }
                    else
                        t.set(loc_x);
                    pb.push_back(t);
                }

                for (auto t : D[i])
                {
                    if (t[loc_1] == 1)
                    {
                        t.set(loc_x);
                        t.reset(loc_1);
                    }
                    else
                        t.set(loc_x);

                    ps.push_back(t);
                }

            }



            b.push_back(pb);
            s.push_back(ps);
        }
        //sort(b[79].begin(), b[79].end(), MySort);
       // sort(s[79].begin(), s[79].end(), MySort);

        cout << "update b: ";
        printPoly(b[79]);
        cout << "update s: ";
        printPoly(s[79]);
    }

    Z.clear();
    Z.push_back(b[1]);
    Z.push_back(b[2]);
    Z.push_back(b[4]);
    Z.push_back(b[10]);
    Z.push_back(b[31]);
    Z.push_back(b[43]);
    Z.push_back(b[56]);
    Z.push_back(b[63]);
    Z.push_back(s[25]);
    Z.push_back(multiply(multiply(s[3], s[25]), s[46]));
    Z.push_back(multiply(multiply(s[3], s[46]), b[63]));
    Z.push_back(multiply(multiply(s[25], s[46]), b[63]));
    F1.clear();
    F1.push_back(s[3]);
    F1.push_back(s[46]);
    F1.push_back(b[63]);
    F1.push_back(multiply(s[3], s[46]));
    F1.push_back(multiply(s[46], b[63]));
    f1 = addmore(F1);
    Z.push_back(multiply(s[64], f1));
    z = addmore(Z);

    vector<Poly>().swap(F1);
    vector<Poly>().swap(Z);
    vector<Poly>().swap(T1);
    vector<Poly>().swap(T2);

    //sort(z.begin(), z.end(), MySort);
    return calculatedifference(z);
}

//   Part 3: Backward Substitution Expression
int find_isolate(Poly poly)
{
    int flag;
    for (int i = round - 1; i >= 0; i--)
    {
        for (int j = 0; j < 4; j++)
        {
            flag = 0;
            for (auto& term : poly)
            {
                if (term[loc_1 + 1 + i + j * round] == 1)
                {
                    flag = flag + term.count();
                    if (flag > 1)
                        break;
                }
            }
            if (flag == 1)
                return loc_1 + 1 + i + j * round;
        }
    }
    return -1;
}
Poly pushback(Poly difference1, vector<Poly>& A, vector<Poly>& B, vector<Poly>& C, vector<Poly>& D)
{
    int isolate;
    Poly poly, condition;

    isolate = find_isolate(difference1);
    while (isolate >= 0)
    {
        if (isolate < loc_1 + 1 + round)
            condition = A[isolate - (loc_1 + 1)];
        else if (isolate < loc_1 + 1 + 2 * round)
            condition = B[isolate - (loc_1 + 1) - round];
        else if (isolate < loc_1 + 1 + 3 * round)
            condition = C[isolate - (loc_1 + 1) - 2 * round];
        else
            condition = D[isolate - (loc_1 + 1) - 3 * round];

        poly.clear();

        for (auto& term : difference1)
        {
            if (term[isolate] != 1)
                poly.push_back(term);
        }
        difference1 = add(poly, condition);
        //cout << isolate << endl;
        //printPoly(difference1);

        isolate = find_isolate(difference1);
    }
    //sort(difference1.begin(), difference1.end(), MySort);
    return difference1;
}

//   Part 4: Evaluating the Bias



int countVariables(Poly& poly)
{
    PolyTerm vars;
    for (auto& term : poly)
        vars |= term;
    return vars.count();
}

// 自定义比较函数，用于按照变元数量从少到多排序
bool comparePolys(Poly& p1, Poly& p2)
{
    return countVariables(p1) < countVariables(p2);
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

    //sort(Separate.begin(), Separate.end(), comparePolys);
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
Poly simplify_f1(int flag, Poly f, int value)
{
    Poly poly;
    if (value == 0)
    {
        for (auto& term : f)
        {
            if (term[flag] == 0)
                poly.push_back(term);
        }

    }
    if (value == 1)
    {
        for (auto& term : f)
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
        for (auto& term : poly)
            occurrences[term]++;

        poly.clear();
        for (auto& pair : occurrences)
            if (pair.second % 2 == 1)
                poly.push_back(pair.first);
    }
    return poly;
}
int calculate_weight(Poly poly)
{
    int i, j;

    PolyTerm variable;
    for (auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;

    vector<int> variables;
    for (i = 0; i < VARIABLE_COUNT; i++)
        if (variable[i] == 1)
            variables.push_back(i);
    int size = variables.size();

    PolyTerm term;
    int flag, value;
    int count = 0;
    for (i = 0; i < pow(2, size); i++)
    {
        term.reset();
        for (j = 0; j < size; j++)
            term[variables[j]] = (i >> j) & 1;

        value = 0;
        for (auto& tttt : poly)
        {
            if (tttt[loc_1] == 1)
                value = value ^ 1;
            else
            {
                flag = 0;
                for (j = 0; j < size; j++)
                {
                    if (tttt[variables[j]] == 1)
                    {
                        if (term[variables[j]] == 1)
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
        //cout << i << "        " << value << endl;
        count = count + value;
    }
    //printPoly(poly);
    //cout << count << "   " << pow(2, size)<< endl;
    return count;
}
double Probability_1(Poly poly)
{
    PolyTerm variable;
    for (auto& term : poly)
        variable = variable | term;
    variable[loc_1] = 0;
    int count = variable.count();
    return (1.0 * calculate_weight(poly) / pow(2, count));
}
double simplifiedcalculatebias(Poly poly)
{
    int i, count;
    vector<Poly> Separate = calculateSeparate(poly);

    for (i = 0; i < Separate.size(); i++)
    {
        PolyTerm variable;
        for (auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        count = variable.count();
        if (count == 1)
            return 0;
        else if (count < 5)
            if (calculate_weight(Separate[i]) / pow(2, count) == 0.5)
                return 0;
    }
    return 1;
}
int preprocess(Poly& poly)
{
    int guesses = 0;
    int loc, flag = 1;

    while (flag == 1)
    {
        flag = 0;
        PolyTerm variable;
        for (auto& term : poly)
            variable = variable | term;
        variable[loc_1] = 0;

        Poly f0, f1;
        double b0, b1;
        for (loc = 0; loc < VARIABLE_COUNT; loc++)
        {
            if (variable[loc] == 1)
            {
                f0 = simplify_f1(loc, poly, 0);
                f1 = simplify_f1(loc, poly, 1);
                b0 = simplifiedcalculatebias(f0);
                b1 = simplifiedcalculatebias(f1);
                if (b0 == 0 && b1 == 0)
                    return -1;
                else if (b0 == 0)
                {
                    poly = f1;
                    guesses = guesses + 1;
                    flag = 1;
                    break;
                }
                else if (b1 == 0)
                {
                    poly = f0;
                    guesses = guesses + 1;
                    flag = 1;
                    break;
                }
            }
        }
    }
    return guesses;
}
double calculatebias(Poly poly)
{
    int i, j, preprocess_value;
    int count;
    double bias;
    PolyTerm variable;
    vector<Poly> Separate = calculateSeparate(poly);

    double epsilon = 0.5;
    for (i = 0; i < Separate.size(); i++)
    {
        //printPoly(Separate[i]);
        variable.reset();
        for (auto& term : Separate[i])
            variable = variable | term;
        variable[loc_1] = 0;

        count = variable.count();
        //cout << "Number of variables: " << count <<endl;
        if (count < 10)
        {
            bias = 0.5 - Probability_1(Separate[i]);
            epsilon = 2 * epsilon * bias;
        }
        else
        {
            preprocess_value = preprocess(Separate[i]);
            if (preprocess_value >= 0)
            {
                if (preprocess_value > 0)
                {
                    variable.reset();
                    for (auto& term : Separate[i])
                        variable = variable | term;
                    variable[loc_1] = 0;
                }
                int minn = 200, min_temp;
                Poly f0, f1, h0, h1;
                for (j = 0; j < VARIABLE_COUNT; j++)
                {
                    if (variable[j] == 1)
                    {
                        f0 = simplify_f1(j, Separate[i], 0);
                        f1 = simplify_f1(j, Separate[i], 1);
                        min_temp = max(cardinality(f0), cardinality(f1));
                        if (min_temp < minn)
                        {
                            minn = min_temp;
                            h0 = f0;
                            h1 = f1;
                        }
                    }
                }
                if (simplifiedcalculatebias(h0) == 1)
                {
                    if (simplifiedcalculatebias(h1) == 1)
                        bias = 0.5 * (calculatebias(h0) + calculatebias(h1));
                    else
                        bias = 0.5 * (calculatebias(h0));
                }
                else
                {
                    if (simplifiedcalculatebias(h1) == 1)
                        bias = 0.5 * (calculatebias(h1));
                    else
                        bias = 0;
                }
                epsilon = 2 * epsilon * bias / pow(2, preprocess_value);
            }
            else if (preprocess_value == -1)
                epsilon = 0;
        }
        if (epsilon == 0)
            return 0;
    };

    return epsilon;
}

int main()
{
    // round 120;   N0 51;   loc 22,47       bias: 0.000113487
    // round 125;   N0 50;   loc 21,46       bias: 6.80587e-06
    // round 126;   N0 51;   loc 22,47       bias: 6.80587e-06
    // round 127;   N0 55;   loc 34,49,59    bias: 4.61936e-07
    // round 128;   N0 55;   loc 35,50,60    bias: 4.61936e-07

    vector<int> loc = { 22,47 };
    vector<Poly> b, s;
    vector<Poly> A(round), B(round), C(round), D(round);

    Poly difference1 = calculateANF(loc, b, s, A, B, C, D);
    cout << "The difference ATF is:" << endl;
    printPoly(difference1);

    printDATF_file(A, B, C, D);

    Poly difference2 = pushback(difference1, A, B, C, D);
    cout << endl << "The ATF after backtracking is: " << endl;
    printPoly(difference2);
    cout << endl;

    clock_t start, end;
    start = clock();
    double bias = calculatebias(difference2);
    end = clock();
    cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "bias: " << bias << endl;

    return 0;
}

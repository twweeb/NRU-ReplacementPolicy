//
//  main.cpp
//  ReplacementPolicy
//
//  Created by Lei Hsiung on 1/7/20.
//  Copyright © 2020 Lei Hsiung. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

ofstream fout;
string outs[] = {"Address bits: ", "Block size: ", "Cache sets: ", "Associativity: ", "Offset bit count: ", "Indexing bit count: ", "Indexing bits: ", ".end", "Total cache miss count: "};

int BinToDec(string str)
{
    int decimalNumber = 0, i = 0, remainder;
    size_t N = str.length();
    while (N--)
    {
        remainder = str[N] - '0';
        decimalNumber += remainder*((int)pow(2,i));
        ++i;
    }
    return decimalNumber;
}

int log_2(int a)
{
    int c = 0;
    while(a!=0){
        a/=2;
        c++;
    }
    
    return c-1;
}

vector<int> readConfig(const string& filename)
{
    ifstream source(filename.c_str(), ifstream::in);
    vector<int> lines;
    string line;
    int line_n = 0, tmp, tmp2;
    while (getline(source, line))
    {
        // Extract number from file
        size_t i = 0;
        for ( ; i < line.length(); i++ )
            if ( isdigit(line[i]) ) break;
        line = line.substr(i, line.length() - i );
        fout << outs[line_n++] + line + "\n";
        lines.push_back(atoi(line.c_str()));
    }
    fout << "\n";
    
    // Offset bit count
    lines.push_back(tmp = log_2(lines[1]));
    // Index bit count
    lines.push_back(tmp2 = log_2(lines[2]));
    fout << outs[4] << tmp << "\n"
         << outs[5] << tmp2 << "\n"
         << outs[6];
    for (int i = 0; i< tmp2; i++)
    {
        if(i != 0) fout << " ";
        fout << tmp + tmp2 - i -1 ;
    }
    
    fout << "\n" << "\n";
    
    return lines;
}

vector<string> readRef(const string& filename)
{
    ifstream source(filename.c_str(), ifstream::in);
    vector<string> lines;
    string line;
    int i = 0;
    while (getline(source, line))
    {
        if (!i++) fout << line << "\n";
        if (isdigit(line[0])) lines.push_back(line);
    }
    return lines;
}

void displayVector(const vector<string> v)
{
    for (int i = 0; i != v.size(); ++i)
    {
        if(i != 0) cout << " ";
        cout << v[i];
    }
    cout << endl;
}

void displayVectors(const vector<int> v)
{
    for (int i = 0; i != v.size(); ++i)
    {
        if(i != 0) cout << " ";
        cout << v[i];
    }
    cout << endl;
}

void reset_nrubit(int cache_num, int total_set_num, int**arr)
{
    for(int i = 0; i < total_set_num; ++i)
        arr[cache_num][i] = 1;
}

void reset_data(int cache_num, int total_set_num, int**arr)
{
    for(int i = 0; i < total_set_num; ++i)
        arr[cache_num][i] = -1;
}

int main (int argc,  char **argv) {
    fout.open ("index.rpt");
    
    // Decide memory config:
    //     M-bit address bits
    //     B-byte block size
    //     E cache sets
    //     A-way set associativity.
    //     Offset
    //     Index Bits
    vector<int> cache = readConfig(argv[1]);
    vector<string> reference = readRef(argv[2]);
    
    //displayVectors(cache);
    //displayVector(reference);
    
    // cache_nru[cache_num][set_num]
    int **cache_nru = (int **)malloc(cache[2] * sizeof(int *)),
        **cache_data = (int **)malloc(cache[2] * sizeof(int *));
    for (int i = 0; i < cache[2]; ++i)
    {
        cache_nru[i] = (int *)malloc(cache[3] * sizeof(int));
        cache_data[i] = (int *)malloc(cache[3] * sizeof(int));
        reset_nrubit(i, cache[3], cache_nru);
        reset_data(i, cache[3], cache_data);
    }
    
    int miss_cnt = 0, idx = 0, find = 0;
    for (int i = 0; i< reference.size(); ++i)
    {
        idx = BinToDec(reference[i].substr(cache[0] - cache[4] - cache[5], cache[5]));
        find = 0;
        for(int j = 0; j < cache[3]; ++j)
        {
            //if(i< 10) cout << (reference[i].substr(0, cache[0] - cache[4] - cache[5]).c_str()) << " " <<cache_data[idx][j] << " " << BinToDec(reference[i].substr(0, cache[0] - cache[4] - cache[5]).c_str()) <<endl;
            if(cache_data[idx][j] == BinToDec(reference[i].substr(0, cache[0] - cache[4] - cache[5])))
            {
                find = 1;
                cache_nru[idx][j] = 0;
                fout << reference[i] <<" hit\n";
                break;
            }
        }
        
        if(!find)
        {
            find = 0;
            miss_cnt += 1;
            fout << reference[i] << " miss\n";
            for(int j = 0; j < cache[3]; ++j)
            {
                if(cache_nru[idx][j] == 1)
                {
                    find = 1;
                    cache_nru[idx][j] = 0;
                    cache_data[idx][j] = BinToDec(reference[i].substr(0, cache[0] - cache[4] - cache[5]));
                    break;
                }
            }
            if(!find)
            {
                reset_nrubit(idx, cache[3], cache_nru);
                cache_nru[idx][0] = 0;
                cache_data[idx][0] = BinToDec(reference[i].substr(0, cache[0] - cache[4] - cache[5]));
            }
        }
    }
    fout << outs[7] << "\n\n" << outs[8] << miss_cnt << "\n";
    fout.close();
    
    return 0;
}

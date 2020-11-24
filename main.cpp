
//Author: tangmengjin
//Date: 2020-11-24 14:10:40
//LastEditTime: 2020-11-24 20:00:49
//LastEditors: tangmengjin
//Description:
//FilePath: /multi-pattern-match/main.cpp
//nothing to say

#include <iostream>
#include <fstream>
#include <vector>
#include "wumanber.h"
#include "aho_corasick.hpp"

#define BIBLE1 "bible.txt"
#define PATTERN1 "dict.txt"
#define RESULT1 "result1.txt"

using namespace std;
namespace ac = aho_corasick;
using trie = ac::trie;

void bench_aho_corasick(map<string, int> &result)
{
    fstream f_bible(BIBLE1);
    string line;
    set<string> input_strings;
    while (getline(f_bible, line))
    {
        input_strings.insert(line);
    }
    f_bible.close();

    vector<string> input_vector(input_strings.begin(), input_strings.end());

    set<string> patterns_aho_corasick;
    fstream f_dict(PATTERN1);
    while (getline(f_dict, line))
    {
        patterns_aho_corasick.insert(line);
    }
    f_dict.close();
    vector<string> pattern_vector(patterns_aho_corasick.begin(), patterns_aho_corasick.end());

    trie t;
    t.case_insensitive();
    t.only_whole_words();
    for (auto &pattern : patterns_aho_corasick)
    {
        t.insert(pattern);
    }
    map<string, int> records;
    size_t count = 0;
    auto start_time = chrono::high_resolution_clock::now();
    for (auto &text : input_vector)
    {
        auto matches = t.parse_text(text);
        count += matches.size();
        for (auto &key : matches)
        {
            if (records.find(key.get_keyword()) != records.end())
            {
                records[key.get_keyword()] = records[key.get_keyword()] + 1;
            }
            else
            {
                records[key.get_keyword()] = 1;
            }
        }
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << "bench_aho_corasick size=" << records.size() << ", cost "
         << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms"
         << std::endl;
    result = records;
    return;
}

void bench_wu_manber(map<string, int> &result)
{
    //step1: init patterns
    vector<string> patterns;
    ifstream pat(PATTERN1);
    string s;
    while (getline(pat, s))
    {
        patterns.push_back(s);
    }
    pat.close();

    //step2: init wumanber
    WuManber wu;
    wu.Init(patterns);

    //step3: find patterns using wumanber in paraell
    ifstream text(BIBLE1);
    map<string, int> result_wumanber;
    auto start_time = chrono::high_resolution_clock::now();
    while (getline(text, s))
    {
        //interface1
        //cout << s << " hit:" << wu.Search(s) << endl;
        //interface2
        vector<string> res;
        wu.Search(s, res);
        for (vector<string>::iterator it = res.begin(); it != res.end(); ++it)
        {
            if (result_wumanber.find(*it) != result_wumanber.end())
            {
                result_wumanber[*it] = result_wumanber[*it] + 1;
            }
            else
            {
                result_wumanber[*it] = 1;
            }
        }
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << "wumanber size=" << result_wumanber.size() << ", cost "
         << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms"
         << std::endl;
    result = result_wumanber;
}

void analyzeResult(map<string, int> resultWumanber, map<string, int> resultAho)
{
    if (resultWumanber.size() != resultAho.size())
    {
        map<string, int> resultBig;
        map<string, int> resultSmall;
        if (resultWumanber.size() < resultAho.size())
        {
            resultBig = resultWumanber;
            resultSmall = resultAho;
        }
        else
        {
            resultBig = resultAho;
            resultSmall = resultWumanber;
        }
        for (map<string, int>::iterator it = resultBig.begin(); it != resultBig.end(); ++it)
        {
            if (resultSmall.find(it->first) == resultSmall.end())
            {
                cout << it->first << " not find" << std::endl;
            }
            else
            {
                if (resultWumanber[it->first] != resultAho[it->first])
                {
                    cout << it->first << " wumanber:" << resultWumanber[it->first]
                         << " aho:" << resultAho[it->first] << std::endl;
                }
            }
        }
    }

    ofstream outFile;
    outFile.open(RESULT1);
    fstream f_dict(PATTERN1);
    string line;
    outFile << "keyword"
            << "\t"
            << "wu-manber"
            << "\t"
            << "aho-corasick"
            << "\n";
    while (getline(f_dict, line))
    {
        if (resultWumanber.find(line) != resultWumanber.end())
        {
            outFile << line << "\t" << resultWumanber[line] << "\t" << resultWumanber[line] << "\n";
        }
        else
        {
            outFile << line << "\t" << 0 << "\t" << 0 << "\n";
        }
    }
    outFile.close();
    f_dict.close();
}

int main(int argc, char *argv[])
{

    map<string, int> result_wumanber;
    bench_wu_manber(result_wumanber);

    //aho_corasick

    map<string, int> result_aho;
    bench_aho_corasick(result_aho);

    analyzeResult(result_wumanber, result_aho);
    return 0;
}

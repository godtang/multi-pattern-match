
//Author: tangmengjin
//Date: 2020-11-24 14:10:40
//LastEditTime: 2020-11-24 19:03:49
//LastEditors: tangmengjin
//Description:
//FilePath: /multi-pattern-match/main.cpp
//nothing to say

#include <iostream>
#include <fstream>
#include <vector>
#include "wumanber.h"
#include "aho_corasick.hpp"

using namespace std;
namespace ac = aho_corasick;
using trie = ac::trie;

size_t bench_aho_corasick(vector<string> text_strings, trie &t, map<string, int> &result)
{
    map<string, int> records;
    size_t count = 0;
    for (auto &text : text_strings)
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
    cout << "bench_aho_corasick " << records.size() << std::endl;
    result = records;
    return count;
}

void analyzeResult(map<string, int> result1, map<string, int> result2)
{
    if (result1.size() != result2.size())
    {
        map<string, int> resultBig;
        map<string, int> resultSmall;
        if (result1.size() < result2.size())
        {
            resultBig = result1;
            resultSmall = result2;
        }
        else
        {
            resultBig = result2;
            resultSmall = result1;
        }
        for (map<string, int>::iterator it = resultBig.begin(); it != resultBig.end(); ++it)
        {
            if (resultSmall.find(it->first) == resultSmall.end())
            {
                cout << it->first << " not find" << std::endl;
            }
            else
            {
                if (result1[it->first] != result2[it->first])
                {
                    cout << it->first << " wumanber:" << result1[it->first]
                         << " aho:" << result2[it->first] << std::endl;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    //step1: init patterns
    vector<string> patterns;
    ifstream pat("patterns");
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
    ifstream text("text");
    map<string, int> result_wumanber;
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
    cout << "wumanber  " << result_wumanber.size() << std::endl;

    //aho_corasick
    fstream f_bible("text");
    string line;
    set<string> input_strings;
    while (getline(f_bible, line))
    {
        input_strings.insert(line);
    }
    f_bible.close();

    vector<string> input_vector(input_strings.begin(), input_strings.end());

    set<string> patterns_aho_corasick;
    fstream f_dict("patterns");
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
    map<string, int> result_aho;
    bench_aho_corasick(input_vector, t, result_aho);

    analyzeResult(result_wumanber, result_aho);
    return 0;
}

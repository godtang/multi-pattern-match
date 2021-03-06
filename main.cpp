
//Author: tangmengjin
//Date: 2020-11-24 14:10:40
//LastEditTime: 2020-12-02 20:22:09
//LastEditors: tangmengjin
//Description:
//FilePath: /multi-pattern-match/main.cpp
//nothing to say

#include <iostream>
#include <fstream>
#include <vector>
#include "wumanber.h"
#include "aho_corasick.hpp"
#include "wu_manber.hpp"

#define BIBLE1 "bible.txt"
#define PATTERN1 "dict.txt"
#define RESULT1 "result1.txt"

#define BIBLE2 "multi.txt"
#define PATTERN2 "multi-pattern.txt"
#define RESULT2 "result2.txt"

using namespace std;
namespace ac = aho_corasick;
using trie = ac::trie;

void bench_aho_corasick(string textFile, string patternFile, map<string, int> &result)
{
    fstream f_bible(textFile);
    string line;
    vector<string> input_vector;
    while (getline(f_bible, line))
    {
        input_vector.push_back(line);
    }
    f_bible.close();

    set<string> patterns_aho_corasick;
    fstream f_dict(patternFile);
    while (getline(f_dict, line))
    {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        patterns_aho_corasick.insert(line);
    }
    f_dict.close();

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

void bench_wu_manber(string textFile, string patternFile, map<string, int> &result)
{
    //step1: init patterns
    vector<string> patterns;
    ifstream pat(patternFile);
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
    ifstream text(textFile);
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

void bench_wu_manber_new(string textFile, string patternFile, map<string, int> &result)
{
    // simple driver program for th Wu - Manber algorithm
    wu_manber::WuManber<char> wuManber;
    vector<string> patternList;
    set<string> patternSet;
    ifstream pat(patternFile);
    string line;
    while (getline(pat, line))
    {
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        patternSet.insert(line);
    }
    pat.close();
    for (set<string>::iterator it = patternSet.begin(); it != patternSet.end(); it++)
    {
        patternList.push_back(*it);
    }

    wuManber.preProcess(patternList);
    ifstream dict(textFile);
    auto start_time = chrono::high_resolution_clock::now();
    string s;
    map<string, int> result_wumanber;
    while (getline(dict, s))
    {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        wuManber.scan(s, result_wumanber,
                      [](const string &text, const string &pattern, size_t indexInPatternList, size_t startIndexInText, map<string, int> &records) {
                          // 在这里再进行全字匹配
                          if (text[startIndexInText] != pattern[0] || text[startIndexInText + pattern.length() - 1] != pattern[pattern.length() - 1])
                          {
                              return;
                          }
                          if (((startIndexInText == 0 ||
                                !std::isalpha(text[startIndexInText - 1])) &&
                               (startIndexInText + pattern.length() == text.length() ||
                                !std::isalpha(text[startIndexInText + pattern.length()]))) ||
                              text[startIndexInText] < 0)
                          {
                              string temp = pattern;
                              transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
                              if (records.find(temp) != records.end())
                              {
                                  records[temp] = records[temp] + 1;
                              }
                              else
                              {
                                  records[temp] = 1;
                              }
                          }
                      });
    }
    auto end_time = chrono::high_resolution_clock::now();
    cout << "bench_wu_manber_new size=" << result_wumanber.size() << ",cost "
         << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms"
         << endl;
    result = result_wumanber;
}

void analyzeResult(string resultFile, string patternFile,
                   map<string, int> resultWumanber, map<string, int> resultAho)
{
    if (resultWumanber.size() != resultAho.size())
    {
        map<string, int> resultBig;
        map<string, int> resultSmall;
        if (resultWumanber.size() > resultAho.size())
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
        return;
    }

    ofstream outFile;
    outFile.open(resultFile);
    fstream f_dict(patternFile);
    string line;
    outFile << "keyword"
            << "\t"
            << "wu-manber"
            << "\t"
            << "aho-corasick"
            << "\n";
    while (getline(f_dict, line))
    {
        string lineSrc = line;
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        if (resultWumanber.find(line) != resultWumanber.end())
        {
            if (resultWumanber[line] == resultAho[line])
            {
                outFile << lineSrc << "\t" << resultWumanber[line] << "\t" << resultAho[line] << "\n";
            }
            else
            {
                outFile << lineSrc << "\t" << resultWumanber[line] << "\t" << resultAho[line] << "\tunmatch\n";
            }
        }
        else
        {
            outFile << lineSrc << "\t" << 0 << "\t" << 0 << "\t not find\n";
        }
    }
    outFile.close();
    f_dict.close();
}

int main(int argc, char *argv[])
{
    map<string, int> result_wumanber;
    map<string, int> result_aho;
    //测试英文
    string test1File = BIBLE1;
    string pattern1File = PATTERN1;
    string result1File = RESULT1;
    //wu_manber
    bench_wu_manber_new(test1File, pattern1File, result_wumanber);

    //aho_corasick
    bench_aho_corasick(test1File, pattern1File, result_aho);

    analyzeResult(result1File, pattern1File, result_wumanber, result_aho);

    //测试中英文
    string test2File = BIBLE2;
    string pattern2File = PATTERN2;
    string result2File = RESULT2;
    //wu_manber
    result_wumanber.clear();
    bench_wu_manber_new(test2File, pattern2File, result_wumanber);

    //aho_corasick
    result_aho.clear();
    bench_aho_corasick(test2File, pattern2File, result_aho);

    analyzeResult(result2File, pattern2File, result_wumanber, result_aho);
    return 0;
}

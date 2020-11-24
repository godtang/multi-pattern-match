
//Author: tangmengjin
//Date: 2020-11-24 14:10:40
//LastEditTime: 2020-11-24 17:24:48
//LastEditors: tangmengjin
//Description:
//FilePath: /multi-pattern-match/wumanber.h
//nothing to say

#ifndef ALGORITHM_WUMANBER_H
#define ALGORITHM_WUMANBER_H
#include <stdint.h>

#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <set>

typedef std::vector<std::string> ResultSetType;
typedef std::pair<unsigned int, int> PrefixIdPairType;
typedef std::vector<PrefixIdPairType> PrefixTableType;

class WuManber
{
public:
    WuManber();
    ~WuManber();
    /**
         * Init Function
         * 
         * @param patterns      pattern list to be matched
         */
    bool Init(std::vector<std::string> &patterns);

    /** 
         * @param text           raw text
         * @param textLength     length of text
         * @param res            string set containing matched patterns
         * 
         * @return value 0: no pattern matchs, n: n patterns matched(n>0)
         */
    int Search(const char *text, const int textLength, ResultSetType &res);

    /**
         * @param  str           raw text
         * @param  res           string set containing matched patterns
         *
         * @return value 0: no pattern matchs, n: n patterns matched(n>0)
         */
    int Search(const std::string &str, ResultSetType &res);

    /**
         * @brief Search text 
         *
         * @return value 0: no pattern matchs, n: n patterns matched(n>0)
         */
    int Search(const char *text, const int textLength);

    /**
         * @brief Search text
         *
         * @return value 0: no pattern matchs, n: n patterns matched(n>0)
         */
    int Search(const std::string &str);

    void setWholeWord(bool val);
    void setCaseSensitivity(bool val);

private:
    // minmum length of patterns
    int32_t mMin;
    // SHIFT table
    std::vector<int32_t> mShiftTable;
    // a combination of HASH and PREFIX table
    std::vector<PrefixTableType> mHashTable;
    // patterns
    std::vector<std::string> mPatterns;
    // size of SHIFT and HASH table
    int32_t mTableSize;
    // size of block
    int32_t mBlock;

    bool bWholeWord;
    bool bCaseSensitivity;
};

#endif

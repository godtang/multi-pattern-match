#include <cmath>
#include <iostream>
#include "wumanber.h"

using namespace std;

/** 
 * @brief   String hash function.
 * 
 * @param str   the string needed to be hashed
 * @param len   length of the substr should be hashed
 * 
 * @return hash code
 */
unsigned int HashCode(const char *str, int len)
{
    unsigned int hash = 0;
    while (*str && len > 0)
    {
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
        --len;
    }
    return (hash & 0x7FFFFFFF);
}

/** 
 * @brief constructor 
 */
WuManber::WuManber() : mMin(0), mTableSize(0), mBlock(3)
{
    //VOID
    bWholeWord = true;
    bCaseSensitivity = false;
}

/**
 * @brief Init
 */
bool WuManber::Init(vector<string> &patterns)
{
    int patternSize = patterns.size();

    //check if no pattern specified
    if (patternSize == 0)
    {
        cerr << "Error: wumanber init failed because no pattern specified." << endl;
        return false;
    }

    // 大小写不敏感，英文字母全部转小写
    vector<string> tempPatterns;
    if (!bCaseSensitivity)
    {
        for (vector<string>::iterator it = patterns.begin(); it != patterns.end(); it++)
        {
            string temp = *it;
            transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
            tempPatterns.push_back(temp);
        }
        patterns = tempPatterns;
    }
    else
    {
        tempPatterns = patterns;
    }

    // 通过set去重
    patterns.clear();
    set<string> tempSet;
    for (vector<string>::iterator it = tempPatterns.begin(); it != tempPatterns.end(); it++)
    {
        tempSet.insert(*it);
    }
    for (set<string>::iterator it = tempSet.begin(); it != tempSet.end(); it++)
    {
        patterns.push_back(*it);
    }
    patternSize = patterns.size();

    //caculate the minmum pattern length
    mMin = patterns[0].length();
    int32_t lenPattern = 0;
    for (int i = 0; i < patternSize; ++i)
    {
        lenPattern = patterns[i].length();
        if (lenPattern < mMin)
        {
            mMin = lenPattern;
        }
    }

    //check if mBlock larger than mMin
    if (mBlock > mMin)
    {
        cerr << "Warning: mBlock is larger than minmum pattern length, reset mBlock to minmum, but it will seriously affect the effiency." << endl;
        mBlock = mMin;
    }

    //choose a suitable mTableSize for SHIFT, HASH table
    int32_t primes[6] = {1003, 10007, 100003, 1000003, 10000019, 100000007};
    vector<int32_t> primeList(&primes[0], &primes[6]);

    int32_t threshold = 10 * mMin;
    for (size_t i = 0; i < primeList.size(); ++i)
    {
        if (primeList[i] > patternSize && primeList[i] / patternSize > threshold)
        {
            mTableSize = primeList[i];
            break;
        }
    }

    //if size of patternList is huge.
    if (0 == mTableSize)
    {
        cerr << "Warning: amount of pattern is very large, will cost a great amount of memory." << endl;
        mTableSize = primeList[5];
    }

    //construct ShiftTable and HashTable, and set default value for SHIFT table
    mPatterns = patterns;
    mHashTable.resize(mTableSize);
    // default value is m-mBlock+1 for shift
    int32_t defaultValue = mMin - mBlock + 1;
    mShiftTable.resize(mTableSize, defaultValue);

    //loop through patterns
    for (int id = 0; id < patternSize; ++id)
    {
        // loop through each pattern from right to left
        for (int index = mMin; index >= mBlock; --index)
        {
            unsigned int hash = HashCode(patterns[id].c_str() + index - mBlock, mBlock) % mTableSize;
            if (mShiftTable[hash] > (mMin - index))
            {
                mShiftTable[hash] = mMin - index;
            }
            if (index == mMin)
            {
                unsigned int prefixHash = HashCode(patterns[id].c_str(), mBlock);
                mHashTable[hash].push_back(make_pair(prefixHash, id));
            }
        }
    }

    return true;
}

/** 
 * @brief destructor
 */
WuManber::~WuManber()
{
    //VOID
}

/**
 * @public
 * @brief search multiple pattern in text at one time
 */
int WuManber::Search(const char *text, const int textLength, ResultSetType &res)
{
    //hit count: value to be returned
    int hits = 0;
    int32_t index = mMin - 1; // start off by matching end of largest common pattern

    int32_t blockMaxIndex = mBlock - 1;
    int32_t windowMaxIndex = mMin - 1;

    while (index < textLength)
    {
        unsigned int blockHash = HashCode(text + index - blockMaxIndex, mBlock);
        blockHash = blockHash % mTableSize;
        int shift = mShiftTable[blockHash];
        if (shift > 0)
        {
            index += shift;
        }
        else
        {
            // we have a potential match when shift is 0
            unsigned int prefixHash = HashCode(text + index - windowMaxIndex, mBlock);
            PrefixTableType &element = mHashTable[blockHash];
            PrefixTableType::iterator iter = element.begin();

            while (element.end() != iter)
            {
                if (prefixHash == iter->first)
                {
                    // since prefindex matches, compare target substring with pattern
                    // we know first two characters already match
                    const char *indexTarget = text + index - windowMaxIndex;    //+mBlock
                    const char *indexPattern = mPatterns[iter->second].c_str(); //+mBlock

                    while (('\0' != *indexTarget) && ('\0' != *indexPattern))
                    {
                        // match until we reach end of either string
                        if (*indexTarget == *indexPattern)
                        {
                            // match against chosen case sensitivity
                            ++indexTarget;
                            ++indexPattern;
                        }
                        else
                            break;
                    }
                    // match succeed since we reach the end of the pattern.
                    if ('\0' == *indexPattern)
                    {
                        string temp = string(mPatterns[iter->second]);
                        if (bWholeWord)
                        {
                            int tempLenth = temp.length();
                            char c1 = text[index - 1];
                            char c2 = text[index + tempLenth - 1];
                            if ((index == 1 || !std::isalpha(text[index - 2])) &&
                                (temp.length() == textLength || !std::isalpha(text[index + temp.length() - 1])))
                            {
                                res.insert(temp);
                                ++hits;
                            }
                        }
                        else
                        {
                            res.insert(temp);
                            ++hits;
                        }
                    }
                } //end if
                ++iter;
            } //end while
            ++index;
        } //end else
    }     //end while

    return hits;
}

/**
 * Search
 */
int WuManber::Search(const string &str, ResultSetType &res)
{
    string temp = str;
    if (!bCaseSensitivity)
    {
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    }
    return Search(temp.c_str(), temp.length(), res);
}

/**
 * Search
 */
int WuManber::Search(const char *text, const int textLength)
{
    //hit count: value to be returned
    int hits = 0;
    int index = mMin - 1; // start off by matching end of largest common pattern

    uint32_t blockMaxIndex = mBlock - 1;
    uint32_t windowMaxIndex = mMin - 1;

    while (index < textLength)
    {
        unsigned int blockHash = HashCode(text + index - blockMaxIndex, mBlock);
        blockHash = blockHash % mTableSize;
        int shift = mShiftTable[blockHash];
        if (shift > 0)
        {
            index += shift;
        }
        else
        {
            // we have a potential match when shift is 0
            unsigned int prefixHash = HashCode(text + index - windowMaxIndex, mBlock);
            //prefixHash = prefixHash % mTableSize;
            PrefixTableType &element = mHashTable[blockHash];
            PrefixTableType::iterator iter = element.begin();

            while (element.end() != iter)
            {
                if (prefixHash == iter->first)
                {
                    // since prefindex matches, compare target substring with pattern
                    // we know first two characters already match
                    const char *indexTarget = text + index - windowMaxIndex;    //+mBlock
                    const char *indexPattern = mPatterns[iter->second].c_str(); //+mBlock

                    while (('\0' != *indexTarget) && ('\0' != *indexPattern))
                    {
                        // match until we reach end of either string
                        if (*indexTarget == *indexPattern)
                        {
                            // match against chosen case sensitivity
                            ++indexTarget;
                            ++indexPattern;
                        }
                        else
                            break;
                    }
                    // match succeed since we reach the end of the pattern.
                    if ('\0' == *indexPattern)
                    {
                        ++hits;
                    }
                } //end if
                ++iter;
            } //end while
            ++index;
        } //end else
    }     //end while

    return hits;
}

int WuManber::Search(const string &str)
{
    return Search(str.c_str(), str.length());
}
void WuManber::setWholeWord(bool val)
{
    bWholeWord = val;
}
void WuManber::setCaseSensitivity(bool val)
{
    bCaseSensitivity = val;
}
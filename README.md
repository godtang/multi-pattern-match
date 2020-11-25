<!--
 * @Author: tangmengjin
 * @Date: 2020-11-24 14:10:40
 * @LastEditTime: 2020-11-25 18:14:28
 * @LastEditors: tangmengjin
 * @Description: 
 * @FilePath: /multi-pattern-match/README.md
 * @nothing to say
-->
# 比多模式匹配
    对比wu-manber算法和aho-corasick算法的效率

## 一些约定
* 文件编码格式为UTF8，换行符为\n,而不是\r\n
* 一个中文为3个字符，则第一个字符必然小于0
* “_”是连接符而不是分隔符

## 代码说明
### 源代码
1. 作业代码和数据保存于[https://github.com/godtang/multi-pattern-match](https://github.com/godtang/multi-pattern-match)
2. wu-manber算法来自[https://github.com/BrtTotty/wu-manber-algorithm-for-chinese](https://github.com/BrtTotty/wu-manber-algorithm-for-chinese)
3. aho-corasick算法来自[https://github.com/cjgdev/aho_corasick](https://github.com/cjgdev/aho_corasick)
### 修改
<font color=abcdef size=3>仅说明算法相关的修改</font>
1. wu-manber
    * 修改“_”连接符
    * 中文处理
    * 忽略大小写
    * 全字匹配

    ```
    -bool WuManber::Init(const vector<string> &patterns)
    +bool WuManber::Init(vector<string> &patterns)
    {
     int patternSize = patterns.size();
 
    @@ -45,6 +47,36 @@ bool WuManber::Init(const vector<string> &patterns)
         return false;
     }
 
    +    // 大小写不敏感，英文字母全部转小写
    +    vector<string> tempPatterns;
    +    if (!bCaseSensitivity)
    +    {
    +        for (vector<string>::iterator it = patterns.begin(); it != patterns.end(); it++)
    +        {
    +            string temp = *it;
    +            transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    +            tempPatterns.push_back(temp);
    +        }
    +        patterns = tempPatterns;
    +    }
    +    else
    +    {
    +        tempPatterns = patterns;
    +    }
    +
    +    // 通过set去重
    +    patterns.clear();
    +    set<string> tempSet;
    +    for (vector<string>::iterator it = tempPatterns.begin(); it != tempPatterns.end(); it++)
    +    {
    +        tempSet.insert(*it);
    +    }
    +    for (set<string>::iterator it = tempSet.begin(); it != tempSet.end(); it++)
    +    {
    +        patterns.push_back(*it);
    +    }
    +    patternSize = patterns.size();
    +
                        // match succeed since we reach the end of the pattern.
                        if ('\0' == *indexPattern)
                        {
    -                        string tempPattern = mPatterns[iter->second];
    -                        char start = text[index - windowMaxIndex - 1];
    -                        char end = text[index - windowMaxIndex + tempPattern.length()];
    -                        res.insert(tempPattern);
    -                        ++hits;
    +                        string temp = string(mPatterns[iter->second]);
    +                        // temp[0] < 0(中文开始的字符串，不考虑全字匹配)
    +                        if (bWholeWord && temp[0] > 0)
    +                        {
    +                            if ((index - windowMaxIndex == 0 ||
    +                                 !std::isalpha(text[index - windowMaxIndex - 1])) &&
    +                                (index - windowMaxIndex + temp.length() == textLength ||
    +                                 !std::isalpha(text[index - windowMaxIndex + temp.length()])))
    +                            {
    +                                res.push_back(temp);
    +                                ++hits;
    +                            }
    +                        }
    +                        else
    +                        {
    +                            res.push_back(temp);
    +                            ++hits;
    +                        }
                        }
                 } //end if
    ```
2. aho-corasick
    * 修改“_”连接符
    * 中文处理

    aho_corasick.hpp
    ```
    emit_collection remove_emits;
    for (const auto &e : collected_emits)
    {
    - if ((e.get_start() == 0 || !std::isalpha(search_text.at(e.get_start() - 1))) &&
    - (e.get_end() + 1 == size || !std::isalpha(search_text.at(e.get_end() + 1))))
    + // 中文开头，无需全字匹配
    + if (search_text.at(e.get_start()) < 0)
    + {
    +     continue;
    + }
    + if ((e.get_start() == 0 || !std::isalpha(search_text.at(e.get_start() - 1))) &&
    +    (e.get_end() + 1 == size || !std::isalpha(search_text.at(e.get_end() + 1)) ))
    {
        continue;
    }
    ```
## 比较结果
1. 考题1
    bible.txt约4M，dict.txt约500K
    结果
    *wumanber cost 19633 ms
    aho_corasick cost 5079 ms*
2. 考题2
    multi-pattern.txt约490K，multi-pattern.txt约50K
    结果
    *wumanber cost 60 ms
    aho_corasick cost 279 ms*
* 可以发现在纯英文和中英文混合的情况下，效率是不一致的




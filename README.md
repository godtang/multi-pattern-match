<!--
 * @Author: tangmengjin
 * @Date: 2020-11-24 14:10:40
 * @LastEditTime: 2020-12-03 10:17:16
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
* “_”是分隔符而不是连接符
* 中文始终是全字匹配的

## 代码说明
### 源代码
1. 作业代码和数据保存于[https://github.com/godtang/multi-pattern-match](https://github.com/godtang/multi-pattern-match)
2. wu-manber算法来自[https://github.com/bubiche/wu_manber](https://github.com/bubiche/wu_manber)
3. aho-corasick算法来自[https://github.com/cjgdev/aho_corasick](https://github.com/cjgdev/aho_corasick)
### 修改
<font color=abcdef size=3>仅说明算法相关的修改</font>
1. wu-manber
    * 中文处理
    * 忽略大小写
    * 全字匹配

    ```
    132c132
    <         if (firstCharacterMatchIndex > -1 && text[0] > 0)
    ---
    >         if (firstCharacterMatchIndex > -1)
    244,247d243
    <       if (text[cur_idx] < 0 || text[cur_idx - 1] < 0)
    <       {
    <         return;
    <       }

        wuManber.scan(s, result_wumanber,
            [](const string &text, const string &pattern, size_t indexInPatternList, size_t startIndexInText, map<string, int> &records) {
                // 在这里再进行全字匹配
            +    if (text[startIndexInText] != pattern[0] || text[startIndexInText + pattern.length() - 1] != pattern[pattern.length() - 1])
            +    {
            +        return;
            +    }
            +    if (((startIndexInText == 0 ||
            +        !std::isalpha(text[startIndexInText - 1])) &&
            +        (startIndexInText + pattern.length() == text.length() ||
            +        !std::isalpha(text[startIndexInText + pattern.length()]))) ||
            +        text[startIndexInText] < 0)
            +    {
            +        string temp = pattern;
            +        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
            +        if (records.find(temp) != records.end())
            +        {
            +            records[temp] = records[temp] + 1;
            +        }
            +        else
            +        {
            +            records[temp] = 1;
            +        }
                }
            });
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
1. <b>考题1</b>
    bible.txt约4M，dict.txt约500K    
    结果    
    *wu_manber cost 3826 ms<br/>aho_corasick cost 4936 ms*    
2. <b>考题2</b>
    multi-pattern.txt约490K，multi-pattern.txt约50K    
    结果    
    *wu_manber cost 17 ms<br/>aho_corasick cost 279 ms*    
* 可以发现在纯英文和中英文混合的情况下，效率是不一致的
* 不能达到要求的wu_manber是aho_corasick的30~50倍的速度



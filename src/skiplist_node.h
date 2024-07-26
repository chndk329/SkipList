/*
 *  程序功能：跳表的结点的封装
 *
 *  作者：DK
 *  日期：2024/07/26
 *
*/

#ifndef __NODE__HH__
#define __NODE__HH__ 1

#include <cstring>
#include <string>
#include <iostream>

// 模板类的定义和实现要放在同一个文件，否则是链接错误
// skiplist_node类声明 ///////////////////////////////////////////////////////////////////////////////////////////////
template<typename K, typename V>
class skiplist_node
{
private:
    const K _key;                   // 结点数据
    V _value;

    int _level;             // 结点的层数

public:
    skiplist_node** _next;                   // 本结点每层的对应的next指针

public:
    skiplist_node(const int);           // 用于创建哨兵结点
    skiplist_node(const K&, const V&, const int);
   ~skiplist_node();

public:
    K key() const;          // 获取结点key
    V& value();             // 获取结点value

    void setValue(const V&);    // 修改value;
};

// 函数实现 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename K, typename V>
skiplist_node<K, V>::skiplist_node(const int level_) : _level(level_)
{
    _next = new skiplist_node<K, V>* [_level];
    memset(_next, 0, sizeof(skiplist_node<K, V>*) * _level);      // 初始化为nullptr
}

template<typename K, typename V>
skiplist_node<K, V>::skiplist_node(const K& key_, const V& value_, const int level_)
                    : _key(key_), _value(value_), _level(level_)
{
    _next = new skiplist_node<K, V>* [_level];
    memset(_next, 0, sizeof(skiplist_node<K, V>*) * _level);      // 初始化为nullptr
}

template<typename K, typename V>
skiplist_node<K, V>::~skiplist_node()
{
    delete[] _next;
}

template<typename K, typename V>
K skiplist_node<K, V>::key() const
{
    return _key;
}

template<typename K, typename V>
V& skiplist_node<K, V>::value()
{
    return _value;
}

template<typename K, typename V>
void skiplist_node<K, V>::setValue(const V& value_)
{
    _value = value_;
}

#endif

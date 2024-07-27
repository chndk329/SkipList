/*
 *  程序功能：跳表的封装
 *
 *  作者：DK
 *  日期：2024/07/26
 *
*/


#ifndef __SKIP_LIST__HH__
#define __SKIP_LIST__HH__ 1

#include "skiplist_node.h"
#include "memorypool.h"

#include <cstdio>
#include <cstdlib>      // rand()函数
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <time.h>

#define STORE_FILE "../store/datafile"

// 模板类的定义和实现要放在同一个文件，否则是链接错误

const std::string delimiter = ":";      // 数据存盘时K,V的分隔符

const int DEF_LEVEL = 20;              // 跳表默认层数
const int MAX_LEVEL = 32;              // 跳表最大层数

// 比较器，默认按升序排序
template<typename T>
struct Less
{
    bool operator() (const T& a, const T& b)
    {
        return a < b;
    }
};

template<typename K, typename V, typename Comp = Less<K>>
class skiplist
{
using node = skiplist_node<K, V>;
public:
    node*  _head;         // 头结点
    node*  _tail;         // 尾结点

    memorypool<node>* _memorypool;    // 内存池

    std::mutex _mutex;          // 互斥锁

    Comp _less;                 // 比较器

private:
    int _level;                 // 跳表的层数
    int _size;                  // 元素个数

private:
    std::ofstream _file_writer;     // dumpfile
    std::ifstream _file_reader;     // loadfile

public:
    skiplist(int level_ = DEF_LEVEL);                  // 跳表的默认层数为20层
    skiplist(Comp less_);
   ~skiplist();

public:
    // 迭代器
    struct Iter
    {
        node* p;

        Iter() : p(nullptr) {}
        Iter(node* rhs_) : p(rhs_) {}

        node* operator-> () const { return (p); }
        node& operator* () const { return *p; }
        bool operator== (const Iter& rhs_) { return rhs_.p == p; }
        bool operator!= (const Iter& rhs_) { return !(rhs_.p == p); }
        void operator++ () { p = p -> _next[0]; }
        void operator++ (int) { p = p -> _next[0]; }
    };

    Iter end() { return _tail; }
    Iter begin() { return _head -> _next[0]; }

    // 重载[]
    V& operator[] (const K& key_) 
    {
        node* update[_level];
        node* p = find(key_, update);

        if(p != _tail && p -> key() == key_) return p -> value();

        // skiplist中没有key_，新建一个空value
        int node_level = randomLevel();      // 获取结点层数
        node *cur = createNode(key_, V(), node_level);
        for(int i = 0; i < node_level; i ++)
        {
            cur -> _next[i] = update[i] -> _next[i];
            update[i] -> _next[i] = cur;
        }
        _size ++;           // 元素个数加1
        return cur -> value();
    }

public:
    // 数据接口
    void insert(const K&, const V&);                    // 插入
    Iter find(const K&);                                // 查询
    bool erase(const K&);                               // 删除
    bool count(const K&);                               // 判断key是否存在
    void clear();                                       // 清空
    int size();                                         // 跳表元素个数

    void load(std::string filename = STORE_FILE);   // 加载磁盘文件
    void dump(std::string filename = STORE_FILE);   // 数据存盘

private:
    void init();

    node* createNode(const K& key_, const V& value_, const int node_level_)
    {
        //return new node(key_, value_, node_level_);
        return new(_memorypool -> alloc()) node(key_, value_, node_level_);
    }

    void deleteNode(node* p_) 
    {
        //delete p_;
        _memorypool -> free(p_);
    }

    // update要求有效，存储每一层的带查询结点的前继
    node* find(const K& key_, node** update_)
    {
        node* cur = _head;
        for(int i = _level - 1; i >= 0; i --)
        {
            while((cur -> _next[i] != _tail) && _less(cur -> _next[i] -> key(), key_)) cur = cur -> _next[i];
            update_[i] = cur;
        }
        return cur -> _next[0];
    }

    int randomLevel();                                  // 获取结点层数
    void free(node*);                                   // 递归释放结点空间

    void getKeyAndValueFromString(const std::string& str, std::string *key, std::string *value);        // 从str中获取key和value
    bool isValidString(const std::string& str);                                                         // 判断str是否存在分隔符delimiter
};

// 函数实现

//========================================================= 构造函数 ===================================================================
template<typename K, typename V, typename Comp>
skiplist<K, V, Comp>::skiplist(const int level_) : _level(std::min(MAX_LEVEL, level_)), _size(0)
{
    init();
}

template<typename K, typename V, typename Comp>
skiplist<K, V, Comp>::skiplist(Comp less_) : _level(DEF_LEVEL), _size(0), _less{less_}
{
    init();
}

template<typename K, typename V, typename Comp>
skiplist<K, V, Comp>::~skiplist()
{
    free(_head -> _next[0]);
    deleteNode(_head);
    deleteNode(_tail);
    delete _memorypool;
}
//=====================================================================================================================================

//========================================================= 辅助函数 ===================================================================
template<typename K, typename V, typename Comp>
int skiplist<K, V, Comp>::randomLevel()
{
    int k = 0;
    while(k < _level)
    {
        k ++;
        if(rand() % 2) break;       // 50%的概率
    }
    return k;
}

template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::free(node* p_)
{
    node *cur = p_;
    while(cur != _tail)
    {
        node* last = cur -> _next[0];
        deleteNode(cur);
        cur = last;
    }
}

template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::init()
{
    srand((uint32_t)time(0));
    _memorypool = new memorypool<node>();         // 先创建内存池

    _head = createNode(K(), V(), _level);
    _tail = createNode(K(), V(), _level);

    for(int i = 0; i < _level; i ++) _head -> _next[i] = _tail;         // 头指针指向尾指针
}
//=====================================================================================================================================

//========================================================= 核心操作 ===================================================================
template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::insert(const K& key_, const V& value_)
{
    std::lock_guard<std::mutex> gd(_mutex);          // 加锁

    node* update[_level];
    node* p = find(key_, update);

    if(p != _tail && p -> key() == key_)
    {
        // 结点存在
        p -> setValue(value_);
    }
    else
    {
        int node_level = randomLevel();      // 获取结点层数

        node *cur = createNode(key_, value_, node_level);
        for(int i = 0; i < node_level; i ++)
        {
            cur -> _next[i] = update[i] -> _next[i];
            update[i] -> _next[i] = cur;
        }
        _size ++;           // 元素个数加1
    }
}

template<typename K, typename V, typename Comp>
typename skiplist<K, V, Comp>::Iter skiplist<K, V, Comp>::find(const K& key_)
{
    std::lock_guard<std::mutex> gd(_mutex);          // 加锁

    node* update[_level];
    node* p = find(key_, update);

    if(p == _tail || p -> key() != key_) return end();
    return skiplist<K, V, Comp>::Iter(p);
}

template<typename K, typename V, typename Comp>
bool skiplist<K, V, Comp>::erase(const K& key_)
{
    std::lock_guard<std::mutex> gd(_mutex);          // 加锁

    node* update[_level];
    node* p = find(key_, update);

    if(p != _tail && p -> key() == key_)
    {
        // 元素存在则删除
        for(int i = 0; i < _level && (update[i] -> _next[i] == p); i ++)
        {
            update[i] -> _next[i] = p -> _next[i];
        }
        deleteNode(p);
        _size --;
        return true;
    }
    return false;       // 删除失败
}

template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::clear()
{
    free(_head -> _next[0]);
    for(int i = 0; i < _level; i ++) _head -> _next[i] = _tail;
    _size = 0;
}

template<typename K, typename V, typename Comp>
int skiplist<K, V, Comp>::size()
{
    return _size;
}

template<typename K, typename V, typename Comp>
bool skiplist<K, V, Comp>::count(const K& key_)
{
    node* update[_level];
    node* p = find(key_, update);

    if(p == _tail) return false;
    return p -> key() == key_;
}
//=====================================================================================================================================

//========================================================= 文件操作 ===================================================================
template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::getKeyAndValueFromString(const std::string& str, std::string *key, std::string *value)
{
    if(isValidString(str) == false) return;
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter) + 1);
}

template<typename K, typename V, typename Comp>
bool skiplist<K, V, Comp>::isValidString(const std::string& str)
{
    if(str.empty()) return false;
    if(str.find(delimiter) == std::string::npos) return false;
    return true;
}

template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::load(std::string filename)
{
    _file_reader.open(filename);
    if(_file_reader.is_open() == false)
    {
        std::cout << "数据文件打开失败" << std::endl;
        return;
    }

    std::string line;
    std::string *key = new std::string();
    std::string *value = new std::string();

    while(getline(_file_reader, line))
    {
        getKeyAndValueFromString(line, key, value);
        if(key -> empty() || value -> empty()) continue;

        // 默认key为int，value为string，后续再优化
        insert(stoi(*key), *value);
    }
    delete key;
    delete value;
    _file_reader.close();
}

template<typename K, typename V, typename Comp>
void skiplist<K, V, Comp>::dump(std::string filename)
{
    _file_writer.open(filename);
    if(_file_writer.is_open() == false)
    {
        std::cout << "数据文件打开失败" << std::endl;
        return;
    }

    node *cur = this -> _head -> _next[0];

    while(cur != _tail)
    {
        _file_writer << cur -> key() << ":" << cur -> value() << "\n";
        cur = cur -> _next[0];
    }

    _file_writer.flush();
    _file_writer.close();
}
//=====================================================================================================================================

#endif

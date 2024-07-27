/*
 *  程序功能：定长内存池
 *
 *  作者：DK
 *  日期：2024/07/26
 *
*/


#ifndef __MEMORY_POOL__HH__
#define __MEMORY_POOL__HH__ 1

// 模板类的定义和实现要放在同一个文件，否则是链接错误
template<class T>
class memorypool
{
private:
    char *_memory;              // 指向内存块的指针
    size_t _remanentBytes;      // 大块内存_memory在切分过程中的剩余字节数
    void *_freelist;            // 自由链表，用来链接归还的空闲的空间

public:
    memorypool() : _memory(nullptr), _remanentBytes(0), _freelist(nullptr) {}

public:
    T *alloc();
    void free(T *);
};


template<class T>
T *memorypool<T>::alloc()
{
    T *obj = nullptr;

    if(_freelist)
    {
        // _freelist不为空, 表示有回收的T大小的小块可以重复利用

        // 头删法，很精妙=============================================
        void *next = *(void **) _freelist;
        obj = (T *)_freelist;
        _freelist = next;
        //================================================================
    }
    else
    {
        // 自由链表中没有块，也就没有可以重复利用的空间

        // _memory中剩余空间小于T的大小的时候再开空间
        if(_remanentBytes < sizeof(T) || _remanentBytes < sizeof(void *))       // 这也包含剩余空间为0的情况
        {
            _remanentBytes = 128 * 1024;     // 开128K的空间
            _memory = (char *)malloc(_remanentBytes);

            if(_memory == nullptr)      // 申请空间失败，抛出异常
            {
                throw std::bad_alloc();
            }
        }

        obj = (T *) _memory;        // 给定一个T类型的大小

        // 判断一下T的大小，小于void*就给一个指针的大小，大于void*就给T的大小
        size_t objSize = sizeof(T) < sizeof(void *) ? sizeof(void *) : sizeof(T);

        _memory += objSize;         // _memory向后移一个T类型的大小
        _remanentBytes -= objSize;  // 空间给出后_remanetBytes减少了T类型的大小

        // new(obj)T;           // 定位new调用构造函数进行初始化
    }

    return obj;
}

template<class T>
void memorypool<T>::free(T* obj)
{
    // 显示调用析构函数进行清理工作
    obj -> ~T();

    // 头插法，很精妙==========================
    *(void **) obj = _freelist;         // 新块指向旧块（或nullptr）
    _freelist = obj;                    // 头指针指向新块
    // ========================================
}

#endif


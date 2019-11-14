/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/



#pragma once
#include <algorithm>
#include <string.h>
#include <assert.h>

/*
 * 环形缓存
 *
 * NOTE: 当只有一个写线程和一个读线程时, 可实现无锁线程安全
 */
template<typename T>
class RingBuff
{
public:
    explicit RingBuff(const RingBuff<T> &o);
    explicit RingBuff(int size);
    RingBuff();
    ~RingBuff();

    void clear();
    bool isEmpty() const;
    bool isFull() const;
    void resize(int size);
    int dataSize() const;
    int space() const;
    int maxSpace() const;
    T at(int index) const;
    T takeTail();
    T takeHead();
    T head(bool *isOK);
    int copy(int index, T *buf, int len) const;
    int pop(int count);
    void push(const T &value);
    void push(const T *data, int len);
    void resetToTail();

private:
    void _newSpace(int size);
    int _dataSizeToEnd() const;
    int _spaceToEnd() const;

    T *_buf;                 // 内存缓冲区
    int _capacity;           // 数据容量(元素个数)
    int _mask;               // 数据容量大小掩码
    int _in;                 // 缓存写索引
    int _out;                // 缓存读索引
    int _validDataNum;       // 只用于计数使用，表示当前push多少个数据，最大为_capacity
};

/*
 * 拷贝构造。
 * o: 目标对象。
 */
template<typename T>
RingBuff<T>::RingBuff(const RingBuff<T> &o)
{
    _capacity = o._capacity;
    _mask = o._mask;
    _in = o._in;
    _out = o._out;
    _validDataNum = o._validDataNum;

    if (_capacity != 0)
    {
        _buf = new T[_capacity];
        ::memcpy(_buf, o._buf, _capacity * sizeof(T));
    }
}

/*
 * 构造缓存
 * size: 缓存数据容量(数据元素个数)
 */
template<typename T>
RingBuff<T>::RingBuff(int size)
    : _buf(NULL)
    , _capacity(0)
    , _mask(0)
    , _in(0)
    , _out(0)
    , _validDataNum(0)
{
    _newSpace(size);
}

/*
 * 构造
 */
template<typename T>
RingBuff<T>::RingBuff()
    : _buf(NULL)
    , _capacity(0)
    , _mask(0)
    , _in(0)
    , _out(0)
    , _validDataNum(0)
{
}

/*
 * 析构，释放缓存
 */
template<typename T>
RingBuff<T>::~RingBuff()
{
    if (_buf)
    {
        delete[] _buf;
        _buf = NULL;
    }
}

/*
 * 申请缓存
 * size: 缓存数据容量(数据元素个数)
 */
template<typename T>
void RingBuff<T>::_newSpace(int size)
{
    if (_buf)
    {
        delete[] _buf;
        _buf = NULL;
    }

    // 保证缓存数据容量为2的幂次方，
    // 以 & 运算替代 % 运算，优化速度。
    int cap = 1;
    while (cap < size)
    {
        cap <<= 1;
    }

//    assert(cap <= 0x8000);  // 小于32K
//    assert(cap <= 0x10000);  // 小于64K
    assert(cap <= 0x20000);  // 小于128K

    _buf = new T[cap];
    _capacity = cap;
    _mask = (_capacity - 1);

    clear();
}

/*
 * 调整缓存大小
 * size: 缓存数据容量(数据元素个数)
 */
template<typename T>
void RingBuff<T>::resize(int size)
{
    // 如果size在(_capacity/2, _capacity]范围内，
    // 清空缓存，直接返回，减少内存分配次数，提升效率
    if ((size > (_capacity >> 1))
            && (size <= _capacity))
    {
        clear();
        return;
    }

    _newSpace(size);
}

/*
 * 清空缓存
 */
template<typename T>
void RingBuff<T>::clear()
{
    _out = _in = 0;
}

/*
 * 判断缓存数据是否为空
 */
template<typename T>
bool RingBuff<T>::isEmpty() const
{
    return (_in == _out);
}

/*
 * 判断缓存是否写满
 */
template<typename T>
bool RingBuff<T>::isFull() const
{
    return (((_in + 1) & _mask) == _out);
}

/*
 * 返回缓存数据元素个数
 */
template<typename T>
int RingBuff<T>::dataSize() const
{
    return ((_in + _capacity - _out) & _mask);
}

/*
 * 返回缓存读索引到缓存末端数据元素个数
 */
template<typename T>
int RingBuff<T>::_dataSizeToEnd() const
{
    int end = _capacity - (_out & _mask);
    return std::min(dataSize(), end);
}

/*
 * 返回缓存可用空闲空间大小(可容纳数据元素的个数)
 */
template<typename T>
int RingBuff<T>::space() const
{
    return (_mask - dataSize());
}

/*
 * 返回缓存最大可用空间大小(可容纳数据元素的个数)
 */
template<typename T>
int RingBuff<T>::maxSpace() const
{
    return _mask;
}

/*
 * 返回缓存写索引到缓存末端可用空闲空间大小(可容纳数据元素的个数)
 */
template<typename T>
int RingBuff<T>::_spaceToEnd() const
{
    int end = _capacity - (_in & _mask);
    return std::min(space(), end);
}

/*
 * 访问缓存中的数据元素
 * index: 数据元素索引, 从0开始
 */
template<typename T>
T RingBuff<T>::at(int index) const
{
    return _buf[(_out + index) & _mask];
}

/*
 * 返回缓存中最老(旧)的数据元素值, 并将其移出缓冲区
 */
template<typename T>
T RingBuff<T>::takeTail()
{
    if (isEmpty())
    {
        return 0;
    }

    T value = _buf[_out];
    _out = (_out + 1) & _mask;

    return value;
}

/*
 * return the newest item and remove it from the buffer
 */
template<typename T>
T RingBuff<T>::takeHead()
{
    if (isEmpty())
    {
        return 0;
    }

    T value = _buf[(_in - 1) & _mask];
    _in = (_in - 1) & _mask;

    return value;
}

/*
 * 返回缓存中最新的数据元素值
 */
template<typename T>
T RingBuff<T>::head(bool *isOK)
{
    if (isEmpty())
    {
        if (isOK)
        {
            *isOK = false;
        }
        return 0;
    }

    if (isOK)
    {
        *isOK = true;
    }
    return _buf[(_in - 1) & _mask];
}

/*
 * 复制缓存中的数据元素
 * index: 欲拷贝数据元素的起始索引，从0开始
 * buf:   存放读取数据元素的内存缓冲区
 * len:   欲读出数据元素的个数
 * 返回值: 实际复制出数据元素的个数
 */
template<typename T>
int RingBuff<T>::copy(int index, T *buf, int len) const
{
    if ((index >= dataSize()) || (NULL == buf) || (0 == len))
    {
        return 0;
    }

    len = std::min((dataSize() - index), len);
    int szToEnd = _dataSizeToEnd();

    if (index >= szToEnd)
    {
        memcpy(buf, (_buf + (index - szToEnd)), len * sizeof(T));
    }
    else
    {
        int n = std::min((szToEnd - index), len);
        int tail = (_out + index) & _mask;
        memcpy(buf, (_buf + tail), n * sizeof(T));

        if (n < len)
        {
            memcpy((buf + n), _buf, (len - n) * sizeof(T));
        }
    }

    return len;
}

/*
 * 移除数据元素
 * count: 移出数据元素的个数  (旧数据元素)
 * 返回值: 实际移出数据元素的个数
 */
template<typename T>
int RingBuff<T>::pop(int count)
{
    int ret = std::min(dataSize(), count);
    _out = (_out + ret) & _mask;
    return ret;
}

/*
 * 向缓存写数据
 * value: 待写数据元素
 */
template<typename T>
void RingBuff<T>::push(const T &value)
{
    if (isFull())
    {
        // 缓存已写满，覆盖写入
        pop(1);
    }

    _buf[_in] = value;
    _in = (_in + 1) & _mask;
    if (_validDataNum < _capacity)
    {
        ++_validDataNum;
    }
}

/*
 * 向缓存写数据
 * data: 待写数据内存缓冲区
 * len:  待写数据元素的个数
 */
template<typename T>
void RingBuff<T>::push(const T *data, int len)
{
    if ((NULL == data) || (0 == len))
    {
        return;
    }

    if (len <= space())
    {
        // 缓存空闲空间足够
        int n = std::min(_spaceToEnd(), len);
        memcpy((_buf + _in), data, n * sizeof(T));

        if (n < len)
        {
            memcpy(_buf, (data + n), (len - n) * sizeof(T));
        }

        _in = (_in + len) & _mask;
        if (_validDataNum + len < _capacity)
        {
            _validDataNum += len;
        }
        else if (_validDataNum < _capacity)
        {
            _validDataNum = _capacity;
        }
        return;
    }

    // 缓存空闲空间不足，覆盖写入
    int count = 0;
    int remain = len;
    while (remain > 0)
    {
        // 一次最多写入 _mask 个数据元素
        int size = std::min(remain, _mask);
        int n = std::min(_spaceToEnd(), size);
        memcpy((_buf + _in), (data + count), n * sizeof(T));

        if (n < size)
        {
            memcpy(_buf, (data + count + n), (size - n) * sizeof(T));
        }

        count += size;
        remain -= size;
        _in = (_in + size) & _mask;
    }

    // 所有数据元素已写入，且缓存已写满
    _out = (_in + 1) & _mask;
    if (_validDataNum + len < _capacity)
    {
        _validDataNum += len;
    }
    else if (_validDataNum < _capacity)
    {
        _validDataNum = _capacity;
    }
}

/*
 * 重置尾部索引
 */
template<typename T>
void RingBuff<T>::resetToTail()
{
    if (_validDataNum < _capacity)
    {
        _out = 0;
    }
    else
    {
        _out = (_in + 1) & _mask;
    }
}

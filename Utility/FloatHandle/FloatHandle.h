#pragma once
#include <typeinfo>
#include <math.h>

// 精度控制, 精度外四舍五入
inline float roundFloat(float value, int decimals = 0)
{
    if(decimals == 0) 
    {
        return roundf(value);
    }
    else 
    {
        float n = powf(10, decimals);
        return (roundf(value * n) / n);
    }
}

// 判断浮点数是否为零
template<typename T>
bool isZero(T value) 
{
    if(typeid(T) == typeid(float))
    {
        return (fabsf(value) < 1e-5);
    }
    else if(typeid(T) == typeid(double))
    {
        return (fabs(value) < 1e-5);
    }
    else if(typeid(T) == typeid(long double))
    {
        return (fabsl(value) < 1e-5);
    }
    else 
    {
        return (value == 0);
    }
}

// 判断浮点数是否相等
template<typename T>
bool isEqual(T a, T b)
{
    if(typeid(T) == typeid(float))
    {
        return (fabsf(a - b) < 1e-5);
    }
    else if(typeid(T) == typeid(double)) 
    {
        return (fabs(a - b) < 1e-5);
    }
    else if(typeid(T) == typeid(long double))
    {
        return (fabsl(a - b) < 1e-5);
    }
    else 
    {
        return (a == b);
    }
}

// 判断浮点数是否相等
template<typename T>
bool isUpper(T a, T b)
{
    if(typeid(T) == typeid(float) ||
            (typeid(T) == typeid(double)) ||
            (typeid(T) == typeid(long double)))
    {
        if ((a - b) > 1e-5)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return (a > b);
    }
}

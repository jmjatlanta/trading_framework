#pragma once
/****
 * Copyright 2022 JMJAtlanta. For licensing details, see LICENSE file
 */
#include <cstdint>
#include <string>
#include <cmath>
#include <list>

/****
 * A way to store floating point numbers with a little less loss and fewer rounding errors
 */
template<class T>
class PrecPrice
{
public:
    PrecPrice(T in) : val_(in), precision_(0) {}
    PrecPrice(double d, uint8_t precision, bool convert=true) 
            : val_( convert?d * std::pow(10, precision):d ), 
            precision_(precision) 
    {}
    double to_double() 
    { 
        if (precision_ == 0)
            return (double)val_;
        return (double)val_ / std::pow(10, precision_); 
    }
    std::string to_string()
    {
        std::string format = "%." + std::to_string(precision_) + "f";
        char temp[35];
        sprintf(temp, format.c_str(), to_double());
        return temp;
    }
    PrecPrice<T> operator+(PrecPrice<T> in)
    {
        if (in.precision_ == precision_)
            return PrecPrice<T>( in.val_ + val_, precision_, false);
        return PrecPrice<T>(in.to_double() + to_double(), precision_);
    }
    PrecPrice<T> operator+(double in)
    {
        PrecPrice<T> temp(in, precision_);
        temp.val_ += val_;
        return temp;
    }
    PrecPrice<T> operator-(PrecPrice<T> in)
    {
        if (in.precision_ == precision_)
            return PrecPrice<T>( val_ - in.val_, precision_, false);
        return PrecPrice<T>(to_double() - in.to_double(), precision_);
    }
    PrecPrice<T> operator-(double in)
    {
        PrecPrice<T> temp(in, precision_);
        temp.val_ = val_ - temp.val;
        return temp;
    }
    PrecPrice<T> operator*(PrecPrice<T> in)
    {
        return PrecPrice<T>(in.to_double() * to_double(), precision_);
    }
    PrecPrice<T> operator*(double in)
    {
        return PrecPrice<T>(in * to_double(), precision_);
    }
    PrecPrice<T> operator/(PrecPrice<T> in)
    {
        // TODO: Handle this with less loss
        return PrecPrice<T>( to_double() / in.to_double(), precision_);
    }
    PrecPrice<T> operator+=(PrecPrice<T> in)
    {
        if( in.precision_ == precision_)
            val_ += in.val_;
        else
        {
            PrecPrice<T> temp(in.to_double, precision_);
            val_ += temp.val_;
        }
        return (*this);
    }
    PrecPrice<T> operator+=(double in)
    {
        PrecPrice<T> temp(in, precision_);
        val_ += temp.val_;
        return *this;
    }
    PrecPrice<T> operator-=(PrecPrice<T> in)
    {
        if (in.precision_ == precision_)
            val_ -= in.val_;
        else
        {
            PrecPrice<T> temp(in.to_double(), precision_);
            val_ -= temp.val_;
        }
        return (*this);
    }
    PrecPrice<T> operator-=(double in)
    {
        PrecPrice<T> temp(in, precision_);
        val_ -= temp.val_;
        return *this;
    }
    template<class R>    
    friend bool operator==(PrecPrice<R>, PrecPrice<R>);
    template<class L, class R>
    friend bool operator==(PrecPrice<L>, PrecPrice<R>);
    template<class R>    
    friend bool operator!=(PrecPrice<R>, PrecPrice<R>);

private:
    T val_;
    uint8_t precision_;
};

template<class T>
bool operator==(PrecPrice<T> lhs, PrecPrice<T> rhs)
{
    if(lhs.precision_ == rhs.precision_)
        return lhs.val_ == rhs.val_;
    return lhs.to_double() == rhs.to_double();
}
template<class L, class R>
bool operator==(PrecPrice<L> lhs, PrecPrice<R> rhs)
{
    return lhs.to_double() == rhs.to_double();
}
template<class T>
bool operator!=(PrecPrice<T> lhs, PrecPrice<T> rhs)
{
    return !(lhs == rhs);
}

typedef PrecPrice<int64_t> Price;
typedef std::list<Price> PriceSeries;
//
//  No2StringBuilder.hpp
//  testStl
//
//  Created by JunAFa on 14-12-6.
//  Copyright (c) 2014年 no2game. All rights reserved.
//


#ifndef testStl_No2StringBuilder_hpp
#define testStl_No2StringBuilder_hpp

#include <iostream>      // for std::cout
#include <fstream>
#include <sstream>
#include <algorithm>     // for for_each
#include <string>        // for std::basic_string;
#include <vector>        // used in main()
#include <deque>         // used in class StringBuilder, slightly faster than list.
#include <numeric>       // for accumulate
#include <ctime>         // clock_t, clock, CLOCKS_PER_SEC

template <typename ch>
class No2StringBuilder {
private:
    typedef std::basic_string<ch> string_t;
    typedef typename string_t::size_type size_type;  // Reuse the size type in the string.
    
    // After testing several times with deque, vector and list, deque performed _slightly_ better.
    typedef std::deque<string_t> container_t;
    
    container_t mData;
    size_type mTotalSize;
    
    void append(const string_t& src)
    {
        mData.push_back(src);
        mTotalSize += src.size();
    }
    
    
    // No copy constructor, no assignment.
    No2StringBuilder(const No2StringBuilder&);
    No2StringBuilder& operator= (const No2StringBuilder&);
    
public:
    No2StringBuilder(const string_t& src)
    {
        if (src.empty())
        {
            mTotalSize = 0;
        }
        else
        {
            mData.push_back(src);
            mTotalSize += src.size();
        }
    }
    
    No2StringBuilder()
    {
        mTotalSize = 0;
    }
    
    No2StringBuilder& appendBack(const string_t& src)
    {
        append(src);
        // allow chaining.
        return *this;
    }
    
    template<class iterator>
    No2StringBuilder& addBack(const iterator& first, const iterator& last)
    {
        // std::for_each and a lambda look like overkill here.
        for (iterator f = first; f != last; ++f) {
            append(*f);
        }
        return *this;
    }
    
    No2StringBuilder& appendLine(const string_t& src)
    {
        ch lineFeed[] = {10, 0};
        mData.push_back(src + lineFeed);
        mTotalSize += 1 + src.size();
        // allow chaining.
        return *this;
    }
    
    No2StringBuilder& appendLine()
    {
        ch lineFeed[] = {10, 0};
        mData.push_back(lineFeed);
        ++mTotalSize;
        // allow chaining.
        return *this;
    }
    
    No2StringBuilder& clear()
    {
        mTotalSize = 0;
        mData.clear();
        return *this;
    }
    
    // ADVANCED TOPIC: AppendFormat()
    // Like C# StringBuilder.ToString()
    string_t toString() const
    {
        string_t result;
        // The whole point of the exercise!
        // If the container has a lot of strings, reallocation (each time the result grows) may take a serious toll,
        // both in performance and chances of failure.
        // I measured (in code I don't own) fractions of a second using 'reserve', and almost two minutes using +=.
        
        result.reserve(1 + mTotalSize); // leave room for the ending zero.
        for (auto iter = mData.begin(); iter != mData.end(); ++iter) {
            result += *iter;
        }
        return result;
    }
    
    
    // Like Javascript array join.
    string_t join(const string_t& delim) const
    {
        if (delim.empty())
        {
            return toString();
        }
        
        string_t result;
        if (mData.empty())
        {
            return result;
        }
        
        // Compute the required size, hoping it won't overflow the size type.
        size_type st = (delim.size() * (mData.size() - 1)) + mTotalSize + 1;
        result.reserve(st);
        
        // Another nice feature in C++11: local structs can be used with STL algorithms.
        struct adder
        {
            string_t mJoiner;
            adder(const string_t& s):mJoiner(s)
            {
                
            }
            
            string_t operator()(string_t& preAllocated, const string_t& current)
            {
                preAllocated += mJoiner;
                preAllocated += current;
                return preAllocated;
            }
        } adr(delim);
        
        auto iter = mData.begin();
        result += *iter;
        return std::accumulate(++iter, mData.end(), result, adr);
    }
};


#endif

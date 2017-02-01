/*
Copyright (c) 2017 Walter William Karas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Definitions for simplifed use of atomic operations in Standard Library.

All atomic memory accesses use relaxed ordering.  Ordering of atomic and
non-atomic memory accesses is done with (free-standing) memory fences.
*/

#ifndef SIMPLE_ATOMIC_20170201
#define SIMPLE_ATOMIC_20170201

#include <atomic>

namespace Simple_atomic
{

// Dummy for overload selection.
class No_threads { };
const No_threads no_threads;

// Atomic type, std::atomic<T_> must be valid.
template <typename T_>
class T
  {
  public:

    // Only use these at startup when only the main thread is running.
    constexpr T(No_threads) : v() { }
    constexpr T(No_threads, T_ v_) : v(v_) { }

    T(T_ v_ = T_()) { store(v_); }
    T(const T &t) { store(t.load()); }

    T & operator = (T_ v_) { store(v_); return(*this); }
    T & operator = (const T &t) { store(t.v); return(*this); }

    T(T &&t) = delete;
    void operator = (T &&t) = delete;

    operator T_ () const { return(load()); }

    T_ operator () () const { return(load()); }

  private:

    std::atomic<T_> v;

    T_ load() const { return(v.load(std::memory_order_relaxed)); }
    void store(T_ v_) { v.store(v_, std::memory_order_relaxed); }

  };

// Memory fences

inline void release() { std::atomic_thread_fence(std::memory_order_release); }

inline void acquire() { std::atomic_thread_fence(std::memory_order_acquire); }

}

#endif // Include once.

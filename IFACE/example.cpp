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

#include "iface.h"

enum Instance
  {
    I_z_x_a
  };

enum Method
  {
    M_z_x_a_1
  };

#define X1_PARAMS(P) \
P(int, p1), \
P(int, p2)

#define X2_PARAMS(P) \
P(double, p1)

#define X_FUNC_LIST(F, FNR) \
F(Instance, who, const, IFACE_NO_PARAMS) \
F(Method, x1, , X1_PARAMS) \
FNR(x2, /*volatile*/, X2_PARAMS)

IFACE_DEF(X, , X_FUNC_LIST)

#define Z_X_FUNC_LIST(F, FNR) \
X_FUNC_LIST(F, FNR)

IFACE_DEF(Z_X, , Z_X_FUNC_LIST)

IFACE_CONVERSION(X, X_FUNC_LIST, Z_X)

class Z_X_A
  {
  private:

    Instance who_;

  public:

    constexpr Z_X_A(Instance w) : who_(w) { }

    Instance who() const { return(who_); }

    Method x1(int, int) { return(M_z_x_a_1); }

    void x2(double) /*volatile*/ { }
  };

IFACE_ENABLE(Z_X, Z_X_FUNC_LIST, Z_X_A)

#include <iostream>

int main()
  {
    Z_X_A z_x_a(I_z_x_a);
    Z_X iface_z_x = iface_factory<Z_X>(z_x_a);

    if (IFACE_CALL_NP(iface_z_x, who) != I_z_x_a)
      std::cout << "BAD\n";

    if (IFACE_CALL(iface_z_x, x1, 0, 0) != M_z_x_a_1)
      std::cout << "BAD\n";

    IFACE_CALL(iface_z_x, x2, 0.0);

    return(0);
  }

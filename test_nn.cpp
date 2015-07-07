/*
 * Copyright (c) 2015 Dropbox, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nn.hpp"

using namespace dropbox::oxygen;
using std::shared_ptr;
using std::unique_ptr;

struct pt_base { virtual ~pt_base() {} };
struct pt : pt_base { int x; int y; pt(int x, int y) : x(x), y(y) {} };

void take_nn_unique_ptr(nn<unique_ptr<int>>) { }
void take_nn_unique_ptr_constref(const nn<unique_ptr<int>> &) { }
void take_unique_ptr(unique_ptr<int>) { }
void take_unique_ptr_constref(const unique_ptr<int> &) { }
void take_base_ptr(nn<unique_ptr<pt_base>>) { }
void take_nn_raw_ptr(nn<int *>) {}
void take_nn_const_raw_ptr(nn<const int *>) {}

int main() {

    // Check that we can operate on raw T* properly
    nn<int*> t = NN_CHECK_ASSERT(new int(7));
    *t = 42;
    nn<pt*> t2 = NN_CHECK_ASSERT(new pt(123, 123));
    t2->x = 1;
    delete static_cast<int*>(t);
    delete static_cast<pt*>(t2);
    delete static_cast<int*>(NN_CHECK_ASSERT(new int(7)));

    // Construct and operate on a unique_ptr
    nn<unique_ptr<pt>> p1 = nn_make_unique<pt>(pt { 2, 2 });
    p1->x = 42;
    *p1 = pt { 10, 10 };
    p1 = nn_make_unique<pt>(pt { 1, 1 });

    // Move a unique_ptr.
    take_nn_unique_ptr(nn_make_unique<int>(1));
    take_nn_unique_ptr_constref(nn_make_unique<int>(1));
    take_unique_ptr_constref(nn_make_unique<int>(1));
    take_unique_ptr_constref(nn_make_unique<int>(1));
    nn<unique_ptr<int>> i = nn_make_unique<int>(42);
    take_nn_unique_ptr_constref(i);
    take_unique_ptr_constref(i);
    // take_unique_ptr(i);
    // take_nn_unique_ptr(i);
    take_nn_unique_ptr(std::move(i));
    i = nn_make_unique<int>(42);
    take_unique_ptr(std::move(i));

    // if (p1) return 0;

    // Construct and operate on a shared_ptr
    nn<shared_ptr<pt>> p2 = nn_make_shared<pt>(pt { 2, 2 });

    p2 = nn_make_shared<pt>(pt { 3, 3 });
    p2->y = 7;
    *p2 = pt { 5, 10 };
    nn<shared_ptr<pt>> p3 = p2;
    shared_ptr<pt> normal_shared_ptr = p3;

    // Check that it still works if const
    const nn<unique_ptr<pt>> c1 = nn_make_unique<pt>(pt { 2, 2 });
    c1->x = 42;
    *c1 = pt { 10, 10 };
    const nn<shared_ptr<pt>> c2 = p2;
    c2->x = 42;
    *c2 = pt { 10, 10 };
    shared_ptr<pt> m2 = c2;

    // Check assignment
    unique_ptr<int> x1;
    shared_ptr<int> x2;
    int * x3;
    {
        /* Work around a Clang bug that causes an ambiguous conversion error here. We have
         * to static_cast<T&&> directly - move() is still ambiguous.
         *
         * http://llvm.org/bugs/show_bug.cgi?id=18359
         */
        x1 = static_cast<unique_ptr<int> &&>(nn_make_unique<int>(1));
        x2 = static_cast<shared_ptr<int> &&>(nn_make_shared<int>(2));
        x3 = NN_CHECK_ASSERT(new int(3));
    }
    delete x3;

    // Check conversions to a base class
    nn<unique_ptr<pt_base>> b1 ( nn_make_unique<pt>(pt { 2, 2 }) );
    nn<shared_ptr<pt_base>> b2 ( p2 );
    b1 = nn_make_unique<pt>(pt { 2, 2 });
    b2 = p2;
    take_base_ptr(nn_make_unique<pt>(pt { 2, 2 }));

    // Check construction of smart pointers from raw pointers
    int * raw1 = new int(7);
    nn<int*> raw2 = NN_CHECK_ASSERT(new int(7));

    unique_ptr<int> u1 (raw1);
    nn<unique_ptr<int>> u2 (raw2);

    // Test comparison
    assert(u1 == u1);
    assert(u2 == u2);
    assert(!(u1 == u2));
    assert(!(u1 != u1));
    assert(!(u2 != u2));
    assert(u1 != u2);
    assert(u1 > u2 || u1 < u2);
    assert(u1 >= u2 || u1 <= u2);

    nn<shared_ptr<int>> shared = move(u2);

    unique_ptr<int> ud1 (new int(7));
    nn<unique_ptr<int>> ud2 ( NN_CHECK_ASSERT(new int(7)) );
//    nn<unique_ptr<int>> ud3 = NN_CHECK_ASSERT(new int(7));
//    u2 = raw2;

    int * this_is_null = nullptr;
    bool threw = false;
    try {
        NN_CHECK_THROW(this_is_null);
    } catch (const std::runtime_error &) {
        threw = true;
    }
    assert(threw);

    int i1 = 42;
    take_nn_raw_ptr(nn_addr(i1));
    take_nn_const_raw_ptr(nn_addr(i1));
    const int i2 = 42;
    //take_nn_raw_ptr(nn_addr(i2));
    take_nn_const_raw_ptr(nn_addr(i2));

    return 0;
}

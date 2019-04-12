//
// Created by isuca on 13-Apr-19.
//
#include <iostream>

using namespace std;

int main() {

#define FILE ""

#ifndef LOCAL
#ifdef FILE
    freopen(FILE".in", "r", stdin);
    freopen(FILE".out", "w", stdout);
#endif
#else
    freopen("__test__", "r", stdin);
#endif

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

}

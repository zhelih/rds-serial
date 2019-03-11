#ifndef __VECTOR_HPP
#define __VECTOR_HPP

typedef unsigned int uint;

#include <vector>
#include <cassert>
#include <cstdio>

class uintvector {
private:
  uint *data;
  int nr;
  int dn;
public:
  uintvector() { data = 0; nr = 0; dn = 0; }
  uintvector(int n) : nr(n), dn(n) { assert(n != 0); data = new uint[n]; }
  uintvector(int n, uint c) : nr(n), dn(n) { assert(n != 0); data = new uint[n]; for(int i = 0; i < n; ++i) data[i] = c; }
  explicit uintvector(const uintvector& uv) { alloc(uv.dn); dn = uv.dn; nr = uv.nr; for(int i = 0; i < nr; ++i) data[i] = uv.data[i]; }
  uintvector& operator=(const uintvector& uv) { alloc(uv.dn); dn = uv.dn; nr = uv.nr; for(int i = 0; i < nr; ++i) data[i] = uv.data[i]; return *this; }
  uintvector(uintvector&& uv) = delete;
  uintvector& operator=(uintvector&& uv) = delete;
  ~uintvector() { if(data) delete [] data; }

  inline void push_back(uint c) { data[nr++] = c; }
  inline uint pop_back() { return data[--nr]; }
  inline int size() const { return nr; }
  inline bool empty() const { return nr == 0; }
  inline void clear() { nr = 0; }
  inline void alloc(int n) { if(!data && n != 0) { data = new uint[n]; dn = n; }; nr = 0; }
  inline uint operator[](int i) const { return data[i]; }
  inline uint& operator[](int i) { return data[i]; }
  inline uint back() const { return data[nr-1]; }
  void to_std(std::vector<uint>& v) { v.resize(nr); for(int i = 0; i < nr; ++i) v[i] = data[i]; }
  inline void resize(int n) { alloc(n); nr = n ; }
  inline uint* d() { return data; }
  void prd() const { printf("obj %p with data %p : nr = %d, reserved %d\n", this, data, nr, dn); }
};

#endif

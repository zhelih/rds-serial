#ifndef _RANK_HPP
#define _RANK_HPP

#include <cstdio>
#include <cfloat>

#define min(a,b) (((a)>(b))?(b):(a))

struct union_of_sets
{
  int* rank;
  int* dad;
  union_of_sets(): rank(nullptr), dad(nullptr) {}
};

union_of_sets InitUnionOfSets(int n)
{
  union_of_sets res;
  res.rank = new int[n];
  res.dad = new int[n];
  return res;
}

void FreeUnionOfSets(union_of_sets& u)
{
  if(u.dad)
    delete [] u.dad;
  if(u.rank)
    delete [] u.rank;
  u.rank = nullptr;
  u.dad = nullptr;
}

void MakeSet(union_of_sets u, int a)
{
  u.rank[a] = 0;
  u.dad[a] = -1;
}

void Union(union_of_sets u, int r1, int r2)
{
  if(u.rank[r1] > u.rank[r2])
    u.dad[r2] = r1;
  else if(u.rank[r1] < u.rank[r2])
    u.dad[r1] = r2;
  else {
    u.dad[r2] = r1;
    u.rank[r1]++;
  }
}

int Find(union_of_sets u, int a)
{
  int r = a;

  while(u.dad[r] != -1)
    r = u.dad[r];

  int y = a;
  // path compression
  if(u.dad[y] != -1)
    while(u.dad[y] != r)
    {
      int prev_dad = u.dad[y];
      u.dad[y] = r;
      y = prev_dad;
    }

  return r;
}

#endif

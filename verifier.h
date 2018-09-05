#ifndef _VERIFIER_H
#define _VERIFIER_H

#include "graph.h"
#include <vector>
#include <map>

typedef unsigned int uint;

class verifier
{
  public:
    virtual bool check_pair(graph* g, uint i, uint j) const = 0;
    virtual bool check(graph* g, const std::vector<uint>& p, uint n) = 0;

    // return aux info for singleton P = { i } and C
    virtual void init_aux(graph* g, uint i, const std::vector<uint>& c) { }
    // return aux info for P u {i}
    // knowing aux for P as prev_aux
    virtual void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c) { }
    virtual void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c) {}
    // free aux info
    virtual void free_aux() {}

    // optional checker
    virtual bool check_solution(graph* g, const std::vector<uint>& res) const { return true; }

    verifier() {}
    virtual ~verifier() {} // do not remove

    // virtual copy constructor
    // careful not to use default copy constructor if using raw pointers
    virtual verifier* clone() const = 0;
};

class clique: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;
  verifier* clone() const;
};

class stable: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;
  verifier* clone() const;
};

class iuc: public verifier
{
  public:
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;
  verifier* clone() const;
};

class defective_clique: public verifier
{
  private:
  uint s;
  uint level;
  uint nnv;
  std::vector<std::vector<uint> > nncnt;

  public:

  defective_clique(uint s_) : s(s_) { }
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;

  void init_aux(graph* g, uint i, const std::vector<uint>& c);
  void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
  void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
  void free_aux();

  verifier* clone() const;
};

class plex: public verifier
{
  private:
  uint s;

  uint level;
  std::vector<std::vector<uint> > nncnt; std::vector<uint> sat; uint nr_sat;
  public:

//  struct t_aux { std::vector<uint> nncnt; std::vector<uint> sat; uint nr_sat; };

  plex(uint s_) : s(s_) { if(s == 0) s = 1; } // s > 0
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;

  void init_aux(graph* g, uint i, const std::vector<uint>& c);
  void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
  void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
  void free_aux();

  verifier* clone() const;
};

class forest: public verifier
{
  private:
  std::vector<uint> color;
  std::vector<uint> parent;
  std::vector<uint> s;
  uint stack_size;
  public:
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);
  bool check_solution(graph* g, const std::vector<uint>& res) const;

  void init_aux(graph* g, uint i, const std::vector<uint>& c);

  verifier* clone() const;
};

class bipartite: public verifier
{
  private:
  std::vector<uint> color;
  std::vector<uint> s;
  uint stack_size;
  public:
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);

  void init_aux(graph* g, uint i, const std::vector<uint>& c);

  verifier* clone() const;
};

class swide: public verifier
{
  private:
  uint s;
  std::vector<uint> is_end; // vector<bool> slower?
  public:
  swide(uint s_) : s(s_) {}
  bool check_pair(graph* g, uint i, uint j) const;
  bool check(graph* g, const std::vector<uint>& p, uint n);

  void init_aux(graph* g, uint i, const std::vector<uint>& c);
  void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
  void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c);
    // free aux info
  void free_aux() {}

  verifier* clone() const;
};

#endif

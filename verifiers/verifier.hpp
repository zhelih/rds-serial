#ifndef _VERIFIER_NEW_H
#define _VERIFIER_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <map>
#include <iostream>
#include <string>
#include <functional>
#include "../graph.h"

class Verifier
{
  public:
    virtual ~Verifier() { free_aux(); }

    virtual bool check_pair(graph* g, uint i, uint j) const = 0;
    virtual bool check(graph* g, const std::vector<uint>& p, uint n) const = 0;
    virtual bool check_solution(graph* g, const std::vector<uint>& res) const = 0;

    // return aux info for singleton P = { i } and C
    virtual void init_aux(graph* g, uint i, const std::vector<uint>& c) { }
    // return aux info for P u {i}
    // knowing aux for P as prev_aux
    virtual void prepare_aux(graph*g, const std::vector<uint>& p, uint i, const std::vector<uint>& c) { }
    virtual void undo_aux(graph* g, const std::vector<uint>& p, uint i, const std::vector<uint>& c) {}
    // free aux info
    virtual void free_aux() {}

    virtual Verifier* clone() const = 0;

    const std::string& getName() { return this->name; }
    const std::string& getShortcut() { return this->shortcut; }
    const std::string& getDescription() { return this->description; }

   protected:
      uint16_t id;
      std::string name;
      std::string description;
      std::string shortcut;
};

template<typename Derived>
class RegisterVerifier: public Verifier
{
   public:
     static Verifier* create() { return new Derived(); }
     static const uint16_t VERIFIER_ID; // for registration
   protected:
      RegisterVerifier() { id = VERIFIER_ID; } //use parameter to instanciate template
};

class VerifierManager
{
  public:
    using Method = std::function<Verifier*()>;

    static VerifierManager *getInstance()
    {
      static VerifierManager inst;
      return &inst;
    }

    uint16_t Register(Method method)
    {
      auto&& verifier = method();
      if (verifiers_by_shortcut.find(verifier->getShortcut()) !=
          verifiers_by_shortcut.end()) {
          printf("Verifier with shortcut %s already exists.",
                 verifier->getShortcut().c_str());
          return 0;
      }
      verifiers[++lastID] = method;
      verifiers_by_name[verifier->getName()] = method;
      verifiers_by_shortcut[verifier->getShortcut()] = method;
      delete verifier;
      return lastID;
    }

    Verifier *create(uint16_t verid)
    {
      return verifiers[verid]();
    }

    Verifier *create(std::string shortcut) {
      return verifiers_by_shortcut[shortcut]();
    }

    std::map<uint16_t, Method> verifiers;
    std::map<std::string, Method> verifiers_by_name;
    std::map<std::string, Method> verifiers_by_shortcut;
    uint16_t lastID = 0;

  private:
    VerifierManager() {};
    VerifierManager(VerifierManager const&) {};
    VerifierManager& operator=(VerifierManager const&);
    ~VerifierManager() {};
};

template <typename Derived>
const uint16_t RegisterVerifier<Derived>::VERIFIER_ID =
  VerifierManager::getInstance()->Register(&RegisterVerifier<Derived>::create);
#endif

#ifndef _VERIFIER_NEW_H
#define _VERIFIER_NEW_H

#include <stdio.h>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include "../graph/graph.h"
#include "../rds/rds_utils.hpp"
#include <iostream>

class verifier
{
  public:
    virtual ~verifier() { free_aux(); }

    virtual bool check_pair(uint i, uint j) const = 0;
    virtual bool check(const std::vector<uint>& p, uint n) const = 0;
    virtual bool check_solution(const std::vector<uint>& res) const = 0;

    // return aux info for singleton P = { i } and C
    virtual void init_aux(uint i, const std::vector<uint>& c) { }
    // return aux info for P u {i}
    // knowing aux for P as prev_aux
    virtual void prepare_aux(const std::vector<uint>& p, uint i, const std::vector<uint>& c) { }
    virtual void undo_aux(const std::vector<uint>& p, uint i, const std::vector<uint>& c) {}
    // free aux info
    virtual void free_aux() {}

    virtual verifier* clone() const = 0;

    const std::string& get_name() { return this->name; }
    const std::string& get_shortcut() { return this->shortcut; }
    const std::string& get_description() { return this->description; }

    unsigned int number_of_parametes() const {
      return number_of_additional_parameters;
    }

    void provide_parameter(const unsigned int& value) {
      if (number_of_provided_parameters < parameters.size())
        *parameters[number_of_provided_parameters] = value;
      ++number_of_provided_parameters;
    }

    const std::string get_parameter_name(const unsigned int number) {
      return parameter_description[number];
    }

    void bind_graph(graph* g) { this->g = g; }

   protected:
      uint16_t id;
      const graph* g;
      std::string name;
      std::string description;
      std::string shortcut;
      uint number_of_additional_parameters = 0;
      uint number_of_provided_parameters = 0;
      std::vector<unsigned int*> parameters;
      std::vector<std::string> parameter_description;

      void add_parameter(const std::string& description, unsigned int* ptr) {
        ++number_of_additional_parameters;
        parameter_description.push_back(description);
        parameters.push_back(ptr);
      }
};

template<typename Derived>
class RegisterVerifier: public verifier
{
   public:
     static verifier* create() { return new Derived(); }
     static const uint16_t VERIFIER_ID; // for registration
   protected:
      RegisterVerifier() { id = VERIFIER_ID; } //use parameter to instanciate template
};

class VerifierManager
{
  public:
    using Method = std::function<verifier*()>;
    using RDSMethod = std::function<algorithm_run(ordering, bool, bool, unsigned int, const std::string&)>;

    static VerifierManager *instance()
    {
      static VerifierManager inst;
      return &inst;
    }

    uint16_t Register(Method method, RDSMethod method_rds)
    {
      auto&& v = method();
      if (verifiers_by_shortcut.find(v->get_shortcut()) !=
          verifiers_by_shortcut.end()) {
          fprintf(stderr, "Verifier with shortcut %s already exists.",
                 v->get_shortcut().c_str());
          return 0;
      }
      verifiers[++lastID] = method;
      verifiers_by_name[v->get_name()] = method;
      verifiers_by_shortcut[v->get_shortcut()] = method;
      verifiers_by_shortcut_rds[v->get_shortcut()] = method_rds;
      delete v;
      return lastID;
    }

    std::shared_ptr<verifier> create(uint16_t verid)
    {
      return std::shared_ptr<verifier>(verifiers[verid]());
    }

    RDSMethod get_rds(const std::string& shortcut) const {
      return verifiers_by_shortcut_rds.at(shortcut);
    }

    size_t count() const {
      return verifiers.size();
    }

    bool is_shortcut(const std::string& shortcut) const {
      return (verifiers_by_shortcut.find(shortcut) != verifiers_by_shortcut.end());
    }

    std::shared_ptr<verifier> create(const std::string& shortcut) const {
      if (verifiers_by_shortcut.find(shortcut) != verifiers_by_shortcut.end())
        return std::shared_ptr<verifier>(verifiers_by_shortcut.at(shortcut)());
      else
        return nullptr;
    }

    std::map<uint16_t, Method> verifiers;
    std::map<std::string, Method> verifiers_by_name;
    std::map<std::string, Method> verifiers_by_shortcut;
    std::map<std::string, RDSMethod> verifiers_by_shortcut_rds;
    uint16_t lastID = 0;

  private:
    VerifierManager() {};
    VerifierManager(VerifierManager const&) {};
    VerifierManager& operator=(VerifierManager const&);
    ~VerifierManager() {};
};

template <typename Derived>
const uint16_t RegisterVerifier<Derived>::VERIFIER_ID =
  VerifierManager::instance()->Register(&RegisterVerifier<Derived>::create, run_rds<Derived>);
#endif

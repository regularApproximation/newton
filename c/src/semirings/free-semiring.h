#pragma once

#include <string>
#include <unordered_map>

#include "../datastructs/hash.h"
#include "../datastructs/matrix.h"
#include "../datastructs/var.h"
#include "../datastructs/free-structure.h"

#include "semiring.h"

template <typename SR>
class Evaluator;

class FreeSemiring : public StarableSemiring<FreeSemiring, Commutativity::NonCommutative, Idempotence::NonIdempotent> {
  public:
    /* Default constructor creates zero element. */
    FreeSemiring() {
      node_ = factory_.GetEmpty();
    }

    FreeSemiring(const VarId var) {
      node_ = factory_.NewElement(var);
    }

    static FreeSemiring null() {
      return FreeSemiring{factory_.GetEmpty()};
    }

    static FreeSemiring one() {
      return FreeSemiring{factory_.GetEpsilon()};
    }

    FreeSemiring star() const {
      return FreeSemiring{factory_.NewStar(node_)};
    }

    FreeSemiring operator+(const FreeSemiring &x) {
      return FreeSemiring{factory_.NewAddition(node_, x.node_)};
    }

    FreeSemiring& operator+=(const FreeSemiring &x) {
      node_ = factory_.NewAddition(node_, x.node_);
      return *this;
    }

    FreeSemiring operator*(const FreeSemiring &x) {
      return FreeSemiring{factory_.NewMultiplication(node_, x.node_)};
    }

    FreeSemiring& operator*=(const FreeSemiring &x) {
      node_ = factory_.NewMultiplication(node_, x.node_);
      return *this;
    }

    bool operator==(const FreeSemiring &x) const {
      return node_ == x.node_;
    }

    std::string string() const {
      return NodeToString(*node_);
    }

    std::string RawString() const {
      return NodeToRawString(*node_);
    }

    template <typename SR>
    SR Eval(const ValuationMap<SR> &valuation) const;

    template <typename SR>
    SR Eval(Evaluator<SR> &evaluator) const;

    void PrintDot(std::ostream &out) {
      factory_.PrintDot(out);
    }

    void PrintStats(std::ostream &out = std::cout) {
      factory_.PrintStats(out);
    }

    void GC() {
      factory_.GC();
    }

  private:
    FreeSemiring(NodePtr n) : node_(n) {}

    NodePtr node_;
    static NodeFactory factory_;

    friend struct std::hash<FreeSemiring>;
};

namespace std {

template <>
struct hash<FreeSemiring> {
  inline std::size_t operator()(const FreeSemiring &fs) const {
    std::hash<NodePtr> h;
    return h(fs.node_);
  }
};

}  /* namespace std */


/*
 * Evaluator
 *
 * We want to memoize the result of evaluating every subgraph, since we can
 * reach the same node (and thus the same subgraph) many times.  So only the
 * first one we will actually perform the computation, in all subsequent visits
 * we will reuse the memoized reslt.  Note that this is ok, because we never
 * modify the actual semiring values.
 */
template <typename SR>
class Evaluator : public NodeVisitor {
  public:
    Evaluator(const ValuationMap<SR> &v)
        : val_(v), evaled_(), result_() {}

    ~Evaluator() {
      for (auto &pair : evaled_) { delete pair.second; }
      /* Top level Node will not be in evaled_. */
      if (result_ != nullptr) {
        /* Why would anyone run the Evaluator and not get the result? */
        assert(false);
        delete result_;
      }
    }

    void Visit(const Addition &a) {
      LookupEval(a.GetLhs());
      auto temp = std::move(result_);
      LookupEval(a.GetRhs());
      result_ = new SR(*temp + *result_);
    }

    void Visit(const Multiplication &m) {
      LookupEval(m.GetLhs());
      auto temp = std::move(result_);
      LookupEval(m.GetRhs());
      result_ = new SR(*temp * *result_);
    }

    void Visit(const Star &s) {
      LookupEval(s.GetNode());
      result_ = new SR(result_->star());
    }

    void Visit(const Element &e) {
      auto iter = val_.find(e.GetVar());
      assert(iter != val_.end());
      result_ = new SR(iter->second);
    }

    void Visit(const Epsilon &e) {
      result_ = new SR(SR::one());
    }

    void Visit(const Empty &e) {
      result_ = new SR(SR::null());
    }

    SR MoveResult() {
      SR tmp = std::move(*result_);
      delete result_;
      result_ = nullptr;
      return tmp;
    }

    static const bool is_idempotent = false;
    static const bool is_commutative = false;

  private:
    void LookupEval(const NodePtr &node) {
      auto iter = evaled_.find(node);
      if (iter != evaled_.end()) {
        result_ = iter->second;
      } else {
        node->Accept(*this);  /* Sets the result_ correctly */
        evaled_.emplace(node, result_);
      }
    }

    const ValuationMap<SR> &val_;
    std::unordered_map<NodePtr, SR*> evaled_;
    SR* result_;
};

template <typename SR>
SR FreeSemiring::Eval(const ValuationMap<SR> &valuation) const {
  Evaluator<SR> evaluator{valuation};
  node_->Accept(evaluator);
  return evaluator.MoveResult();
}

template <typename SR>
SR FreeSemiring::Eval(Evaluator<SR> &evaluator) const {
  node_->Accept(evaluator);
  return evaluator.MoveResult();
}

#include "stdinc.h"
#include "BigIdeal.h"

#include "VarNames.h"
#include "TermTranslator.h"
#include "Ideal.h"

#include <map>

BigIdeal::BigIdeal() {
}

BigIdeal::BigIdeal(const VarNames& names):
  _names(names) {
}

void BigIdeal::insert(const Ideal& ideal) {
  Ideal::const_iterator it = ideal.begin();
  for (; it != ideal.end(); ++it) {
    newLastTerm();

    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = (*it)[var];
  }
}

void BigIdeal::insert(const Ideal& ideal,
		      const TermTranslator& translator) {
  Ideal::const_iterator it = ideal.begin();
  for (; it != ideal.end(); ++it) {
    newLastTerm();

    for (size_t var = 0; var < _names.getVarCount(); ++var)
      getLastTermExponentRef(var) = translator.getExponent(var, (*it)[var]);
  }
}

void BigIdeal::setNames(const VarNames& names) {
  _names = names;
}

void BigIdeal::newLastTerm() {
  ASSERT(!_names.empty());
  
  _terms.resize(_terms.size() + 1);
  _terms.back().resize(_names.getVarCount());
}

mpz_class& BigIdeal::getLastTermExponentRef(size_t var) {
  ASSERT(!empty());
  ASSERT(var < _names.getVarCount());
  
  return _terms.back()[var];
}

bool BigIdeal::operator==(const BigIdeal& b) const {
  return _terms == b._terms;
}

vector<mpz_class>& BigIdeal::operator[](unsigned int index) {
  ASSERT(index < _terms.size());
  return _terms[index];
}

const vector<mpz_class>& BigIdeal::operator[](unsigned int index) const {
  ASSERT(index < _terms.size());
  return _terms[index];
}

bool BigIdeal::empty() const {
  return _terms.empty();
}

void BigIdeal::clear() {
  _terms.clear();
}

size_t BigIdeal::getGeneratorCount() const {
  return _terms.size();
}

size_t BigIdeal::getVarCount() const {
  return _names.getVarCount();
}

void BigIdeal::clearAndSetNames(const VarNames& names) {
  clear();
  _names = names;
}

const VarNames& BigIdeal::getNames() const {
  return _names;
}

void BigIdeal::sortGeneratorsUnique() {
  sortGenerators();
  vector<vector<mpz_class> >::iterator newEnd =
    unique(_terms.begin(), _terms.end());
  _terms.erase(newEnd, _terms.end());
}

void BigIdeal::sortGenerators() {
  std::sort(_terms.begin(), _terms.end(), bigTermCompare);
}

struct VarSorter {
  VarSorter(VarNames& names):
    _names(names) {
    for (size_t i = 0; i < names.getVarCount(); ++i)
      _permutation.push_back(i);
    sort(_permutation.begin(), _permutation.end(), *this);
  }

  bool operator()(size_t a, size_t b) {
    return
      _names.getName(_permutation[a]) <
      _names.getName(_permutation[b]);
  }

  void getOrderedNames(VarNames& names) {
    names.clear();
    for (size_t i = 0; i < _permutation.size(); ++i)
      names.addVar(_names.getName(_permutation[i]));
  }

  void permute(vector<mpz_class>& term) {
    _tmp = term;
    for (size_t i = 0; i < _permutation.size(); ++i)
      term[i] = _tmp[_permutation[i]];
  }

private:
  vector<size_t> _permutation;
  VarNames _names;
  vector<mpz_class> _tmp;
};

void BigIdeal::sortVariables() {
  VarSorter sorter(_names);
  sorter.getOrderedNames(_names);
  for (size_t i = 0; i < _terms.size(); ++i)
    sorter.permute(_terms[i]);
}

void BigIdeal::print(FILE* file) const {
  fprintf(stderr, "/---- BigIdeal of %lu terms:\n",
	  (unsigned long)_terms.size());
  for (vector<vector<mpz_class> >::const_iterator it = _terms.begin();
       it != _terms.end(); ++it) {
    for (vector<mpz_class>::const_iterator entry = it->begin();
	 entry != it->end(); ++entry)
      gmp_fprintf(file, "%Zd ", entry->get_mpz_t());
    fputc('\n', file);
  }
  fputs("----/ End of list.\n", file);
}

const mpz_class& BigIdeal::getExponent(size_t term, size_t var) const {
  ASSERT(term < _terms.size());
  ASSERT(var < _names.getVarCount());

  return _terms[term][var];
}

mpz_class& BigIdeal::getExponent(size_t term, size_t var) {
  ASSERT(term < _terms.size());
  ASSERT(var < _names.getVarCount());

  return _terms[term][var];
}

void BigIdeal::setExponent(size_t term, size_t var, const mpz_class& exp) {
  ASSERT(term < _terms.size());
  ASSERT(var < _names.getVarCount());

  _terms[term][var] = exp;
}

bool BigIdeal::bigTermCompare(const vector<mpz_class>& a,
			      const vector<mpz_class>& b) {
  ASSERT(a.size() == b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] > b[i])
      return true;
    if (a[i] < b[i])
      return false;
  }
  return false;
}

/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "TermTranslator.h"

#include "Term.h"
#include "Ideal.h"
#include "BigIdeal.h"
#include "VarNames.h"

#include <iterator>
#include <sstream>
#include <set>
#include <algorithm>

TermTranslator::TermTranslator(const BigIdeal& bigIdeal, Ideal& ideal,
			       bool sortVars) {
  vector<BigIdeal*> bigIdeals;
  bigIdeals.push_back(const_cast<BigIdeal*>(&bigIdeal));
  initialize(bigIdeals, sortVars);

  shrinkBigIdeal(bigIdeal, ideal);
}

TermTranslator::TermTranslator(const vector<BigIdeal*>& bigIdeals,
							   vector<Ideal*>& ideals) {
  ASSERT(!bigIdeals.empty());

  initialize(bigIdeals, true);

  for (size_t i = 0; i < bigIdeals.size(); ++i) {
    ideals.push_back(new Ideal());
    shrinkBigIdeal(*(bigIdeals[i]), *(ideals.back()));
  }
}

// Helper function for extractExponents.
bool mpzClassPointerLess(const mpz_class* a, const mpz_class* b) {
  return *a < *b;
}

// Helper function for extractExponents.
bool mpzClassPointerEqual(const mpz_class* a, const mpz_class* b) {
  return *a == *b;
}

// Helper function for initialize.
//
// Assign int IDs to big integer exponents. The correspondence
// preserves order, except that the largest ID maps to 0, which is
// necessary to support adding pure powers. Only the exponents
// that actually appear in generators of the ideals are translated,
// except that 0 is guaranteed to be included and to be assigned the
// ID 0, and that a maximal ID is added, which also maps to zero.
//
// extractExponents changes the exponents that it extracts.
void extractExponents(const vector<BigIdeal*>& ideals,
					  vector<mpz_class>& exponents,
					  const string& varName) {
  vector<mpz_class*> exponentRefs;

  mpz_class zero(0);
  exponentRefs.push_back(&zero); // 0 must be included

  // Reserve sufficient capacity for the exponentRefs.
  size_t termCount = 0;
  for (size_t i = 0; i < ideals.size(); ++i)
    termCount += ideals[i]->getGeneratorCount();
  exponentRefs.reserve(termCount + 1); // + 1 because we added the 0 above.

  // Collect the exponents
  const int MaxSmall = 900;
  bool seen[MaxSmall + 1]; // avoid adding small numbers more than once
  fill_n(seen, MaxSmall + 1, false);
  seen[0] = true;
  for (size_t i = 0; i < ideals.size(); ++i) {
    BigIdeal& ideal = *(ideals[i]);
    size_t var = ideal.getNames().getIndex(varName);
    if (var == VarNames::UNKNOWN)
      continue;

    size_t generatorCount = ideal.getGeneratorCount();
    for (size_t term = 0; term < generatorCount; ++term) {
	  const mpz_class& e = ideal.getExponent(term, var);
	  if (e <= MaxSmall) {
		ASSERT(e.fits_uint_p());
		unsigned int ui = e.get_ui();
		if (seen[ui])
		  continue;
		seen[ui] = true;
	  }
	  exponentRefs.push_back(&(ideal.getExponent(term, var)));
	}
  }

  // Sort and remove duplicates.
  std::sort(exponentRefs.begin(), exponentRefs.end(), mpzClassPointerLess);
  exponentRefs.erase
    (unique(exponentRefs.begin(), exponentRefs.end(), mpzClassPointerEqual),
     exponentRefs.end());
  exponentRefs.push_back(&zero);

  exponents.clear();
  exponents.resize(exponentRefs.size());
  size_t size = exponentRefs.size();
  for (size_t e = 0; e < size; ++e)
    exponents[e] = *(exponentRefs[e]);
}

void TermTranslator::clearStrings() {
  for (size_t i = 0; i < _stringExponents.size(); ++i)
    for (size_t j = 0; j < _stringExponents[i].size(); ++j)
      delete[] _stringExponents[i][j];
  _stringExponents.clear();

  for (size_t i = 0; i < _stringVarExponents.size(); ++i)
    for (size_t j = 0; j < _stringVarExponents[i].size(); ++j)
      delete[] _stringVarExponents[i][j];
  _stringVarExponents.clear();
}

void TermTranslator::initialize(const vector<BigIdeal*>& bigIdeals,
				bool sortVars) {
  ASSERT(!bigIdeals.empty());

  if (sortVars) {
    set<string> variables;
    for (size_t ideal = 0; ideal < bigIdeals.size(); ++ideal)
      for (size_t var = 0; var < bigIdeals[ideal]->getVarCount(); ++var)
	variables.insert(bigIdeals[ideal]->getNames().getName(var));
    
    for (set<string>::const_iterator var = variables.begin();
	 var != variables.end(); ++var)
      _names.addVar(*var);
  } else {
    ASSERT(bigIdeals.size() == 1);
    _names = bigIdeals[0]->getNames();
  }

  _exponents.resize(_names.getVarCount());

  for (size_t var = 0; var < _names.getVarCount(); ++var)
    extractExponents(bigIdeals, _exponents[var], _names.getName(var));
}

void TermTranslator::shrinkBigIdeal(const BigIdeal& bigIdeal,
				    Ideal& ideal) const {
  ideal.clearAndSetVarCount(_names.getVarCount());

  // Figure out how bigIdeal's names map onto _names.
  vector<size_t> newVars;
  for (size_t var = 0; var < bigIdeal.getVarCount(); ++var) {
    const string& name = bigIdeal.getNames().getName(var);
    size_t newVar = _names.getIndex(name);
    newVars.push_back(newVar);

    ASSERT(newVar != VarNames::UNKNOWN);
  }

  // Insert generators after translating exponents and variables.
  Term term(ideal.getVarCount());
  size_t varCount = bigIdeal.getVarCount();
  for (size_t i = 0; i < bigIdeal.getGeneratorCount(); ++i) {
    for (size_t var = 0; var < varCount; ++var) {
      size_t newVar = newVars[var];
      term[newVar] = shrinkExponent(newVar, bigIdeal.getExponent(i, var));
    }
    ideal.insert(term);
  }
}

void TermTranslator::addPurePowersAtInfinity(Ideal& ideal) const {
  size_t varCount = ideal.getVarCount();

  // Find out which variables already have pure powers.
  vector<bool> hasPurePower(varCount);

  Ideal::const_iterator stop = ideal.end();
  for (Ideal::const_iterator term = ideal.begin(); term != stop; ++term) {
    if (getSizeOfSupport(*term, varCount) > 1)
      continue;

    size_t var = getFirstNonZeroExponent(*term, varCount);
    if (var == varCount)
      return; // The ideal is <1> so we need add nothing.

    hasPurePower[var] = true;
  }

  // Add any missing pure powers.
  for (size_t var = 0; var < varCount; ++var) {
    if (hasPurePower[var])
      continue;

    Term purePower(varCount);
    purePower[var] = _exponents[var].size() - 1;
    ideal.insert(purePower);
  }
}

void TermTranslator::dualize(const vector<mpz_class>& a) {
  clearStrings();
  for (size_t var = 0; var < _exponents.size(); ++var)
    for (size_t exp = 0; exp < _exponents[var].size(); ++exp)
	  if (_exponents[var][exp] != 0)
		_exponents[var][exp] = a[var] - _exponents[var][exp] + 1;
}

void TermTranslator::decrement() {
  clearStrings();
  for (size_t var = 0; var < _exponents.size(); ++var)
    for (size_t exp = 0; exp < _exponents[var].size(); ++exp)
	  _exponents[var][exp] -= 1;
}

void TermTranslator::renameVariables(const VarNames& names) {
  ASSERT(getVarCount() == names.getVarCount());

  clearStrings();
  _names = names;
}

void TermTranslator::swapVariables(size_t a, size_t b) {
  ASSERT(a < getVarCount());
  ASSERT(b < getVarCount());

  if (a == b)
	return;

  std::swap(_exponents[a], _exponents[b]);

  if (!_stringExponents.empty())
	std::swap(_stringExponents[a], _stringExponents[b]);

  if (!_stringVarExponents.empty())
	std::swap(_stringVarExponents[a], _stringVarExponents[b]);

  _names.swapVariables(a, b);
}

void TermTranslator::print(FILE* file) const {
  fputs("TermTranslator(\n", file);
  for (size_t variable = 0; variable < _exponents.size(); ++variable) {
    fprintf(file, " variable %lu: ", (unsigned long)(variable + 1));
    for (size_t e = 0; e < _exponents[variable].size(); ++e) {
      if (e != 0)
		fputc(' ', file);
      gmp_fprintf(file, "%Zd", _exponents[variable][e].get_mpz_t());
    }
    fputc('\n', file);
  }
  fputs(")\n", file);
}

void TermTranslator::makeStrings(bool includeVar) const {
  vector<vector<const char*> >& strings =
	includeVar ? _stringVarExponents : _stringExponents;

  ASSERT(strings.empty());

  strings.resize(_exponents.size());
  for (unsigned int i = 0; i < _exponents.size(); ++i) {
    strings[i].resize(_exponents[i].size());
    for (unsigned int j = 0; j < _exponents[i].size(); ++j) {
      char* str = 0;

      if (_exponents[i][j] != 0 || !includeVar) {
		stringstream out;
		if (!includeVar)
		  out << _exponents[i][j];
		else {
		  out << _names.getName(i);
		  if (_exponents[i][j] != 1)
			out << '^' << _exponents[i][j];
		}

		str = new char[out.str().size() + 1];
		strcpy(str, out.str().c_str());
      }
      strings[i][j] = str;
    }
  }
}

TermTranslator::~TermTranslator() {
  clearStrings();
}

const mpz_class& TermTranslator::
getExponent(size_t variable, Exponent exponent) const {
  ASSERT(variable < _exponents.size());
  ASSERT(exponent < _exponents[variable].size());

  return _exponents[variable][exponent];
}

const char* TermTranslator::
getVarExponentString(size_t variable, Exponent exponent) const {
  ASSERT(variable < _exponents.size());
  ASSERT(exponent < _exponents[variable].size());

  if (_stringVarExponents.empty())
	makeStrings(true);
  return _stringVarExponents[variable][exponent];
}

const char* TermTranslator::
getExponentString(size_t variable, Exponent exponent) const {
  ASSERT(variable < _exponents.size());
  ASSERT(exponent < _exponents[variable].size());

  if (_stringExponents.empty())
	makeStrings(false);
  return _stringExponents[variable][exponent];
}

const mpz_class& TermTranslator::
getExponent(size_t variable, const Term& term) const {
  return getExponent(variable, term[variable]);
}

Exponent TermTranslator::getMaxId(size_t variable) const {
  ASSERT(variable < _exponents.size());
  
  return _exponents[variable].size() - 1;
}

Exponent TermTranslator::shrinkExponent(size_t var,
				    const mpz_class& exponent) const {
  const vector<mpz_class>& exponents = _exponents[var];

  // We subtract 1 from exponents.end() to skip past the 0 that is
  // added there. Otherwise the range would not be sorted.
  vector<mpz_class>::const_iterator it =
    lower_bound(exponents.begin(), exponents.end() - 1, exponent);
  ASSERT(*it == exponent);

  return it - exponents.begin();
}

const VarNames& TermTranslator::getNames() const {
  return _names;
}

size_t TermTranslator::getVarCount() const {
  return _names.getVarCount();
}

bool TermTranslator::
lessThanReverseLex(const Exponent* a, const Exponent* b) const {
  size_t varCount = getVarCount();

  for (size_t var = 0; var < varCount; ++var) {
	const mpz_class& ae = getExponent(var, a[var]);
	const mpz_class& be = getExponent(var, b[var]);

    if (ae != be)
	  return ae > be;
  }

  return 0;
}

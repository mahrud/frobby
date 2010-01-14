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
#ifndef IDEAL_GUARD
#define IDEAL_GUARD

class Term;

#include <vector>
#include <algorithm>
#include <ostream>

/// Represents a monomial ideal with int exponents.
class Ideal {
  typedef vector<Exponent*> Cont;

 public:
  /** Initialize this object to the zero ideal in varCount variables. */
  Ideal(size_t varCount = 0);

  /** Initialize this object to the ideal generated by term. */
  explicit Ideal(const Term& term);

  /** Initialize this object to be a copy of ideal. */
  Ideal(const Ideal& ideal);
  ~Ideal();

  // *** Accessors

  typedef Cont::const_iterator const_iterator;
  typedef Cont::iterator iterator;
  typedef Cont::const_reverse_iterator const_reverse_iterator;
  typedef Cont::reverse_iterator reverse_iterator;

  const_iterator begin() const {return _terms.begin();}
  const_iterator end() const {return _terms.end();}
  const Exponent* operator[](size_t index) const {return _terms[index];}

  iterator begin() {return _terms.begin();}
  iterator end() {return _terms.end();}
  Exponent*& operator[](size_t index) {return _terms[index];}

  size_t getVarCount() const {return _varCount;}
  size_t getGeneratorCount() const {return _terms.size();}

  bool isIncomparable(const Exponent* term) const;

  // Returns true if any generator divides term.
  bool contains(const Exponent* term) const;

  // Returns true if the identity is one of the generators.
  bool containsIdentity() const;

  // Returns true if some minimal generator strictly divides term.
  bool strictlyContains(const Exponent* term) const;

  // Returns true if no generator divides another.
  bool isMinimallyGenerated() const;

  // Returns true if there are no generators.
  bool isZeroIdeal() const;

  // Returns true if all generators are pure powers. This only
  // corresponds to the mathematical definition of an irreducible
  // polynomial ideal if the ideal is minimally generated.
  bool isIrreducible() const;

  // Returns true if all generators are square free.
  bool isSquareFree() const;

  // Returns true if no non-zero exponent of the same variable appears
  // in two distinct generators. This only corresponds to the
  // mathematical definition of strongly generic if the ideal is
  // minimally generated. This method is not const because it permutes
  // the generators.
  bool isStronglyGeneric();

  // Returns true if for every pair of distinct generators a and b
  // with a[i]=b[i]>0 there is some third generator that strictly
  // divides lcm(a,b). This only corresponds to the mathematical
  // definition of weak genericity if the ideal is minimally
  // generated.
  bool isWeaklyGeneric() const;

  /** Returns true if all pairs of generators have disjoint support.
   I.e. if each var divides at most one generator.
  */
  bool disjointSupport() const;

  /** Sets lcm to the least common multiple of all generators. If
   there are no generators then we define the lcm as the identity. */
  void getLcm(Exponent* lcm) const;

  /** Sets gcd to the greatest common divisor of all generators. If
   there are no generators then we define the gcd as the identity. */
  void getGcd(Exponent* gcd) const;

  /** Sets gcd to the greatest common divisor of those generators that
   raise the variable var to the power exp. If there are no such
   generators then we define the gcd as the identity.
  */
  void getGcdAtExponent(Exponent* gcd, size_t var, Exponent exp);

  /** Sets gcd to the greatest common divisor of those generators that
   are divisible by divisor. If there are no such generators then we
   define the gcd as the identity.
   */
  void getGcdOfMultiplesOf(Exponent* gcd, const Exponent* divisor);

  // least[var] will be the smallest non-zero exponent of var that
  // appears among the generators.
  void getLeastExponents(Exponent* least) const;

  /** counts[var] will be the number of generators divisible by var.
   @todo Make counts a vector<size_t>.
   */
  void getSupportCounts(Exponent* counts) const;

  /** Sets var and exp such that var^exp is the typical non-zero
   exponent. The typical exponent is the exponent of a specific
   variable that occurs most often among the generators. In case of
   ties some deterministic choice is made. If there are no non-zero
   generators then var=exp=0 is chosen and the return value is
   zero. The returned value is the number of times the typical
   non-zero exponent occurs.

   This method is not const since it may re-arrange the generators for
   efficiency.
  */
  size_t getTypicalExponent(size_t& var, Exponent& exp);

  /** Sets var and exp such that var^exp is the most non-generic
   degree. A unordered pair of generators a and b is non-generic if a
   and b raise some var to the same non-zero exponent and lcm(a,b) is
   not strictly divisible by any generator. The most non-generic
   degree is the exponent that occurs most often as the shared degree
   for non-generic pairs. In case of ties some deterministic choice is
   made.

   The return value is the number of non-generic pairs that concern
   the most non-generic degree. If there are no non-generic pairs
   (i.e. the ideal is generic) then the return value is zero and
   var=exp=0.

   This method is not const since it may re-arrange the generators for
   efficiency.
  */
  size_t getMostNonGenericExponent(size_t& var, Exponent& exp);

  /** Sets var and exp such that var^exp is the typical non-generic
   degree. A unordered pair of generators a and b is non-generic if a
   and b raise some var to the same non-zero exponent and lcm(a,b) is
   not strictly divisible by any generator. A degree var^exp is
   non-generic if it occurs as the shared degree for some non-generic
   pair. The typical non-generic degree is the non-generic degree that
   occurs most often among all generators. So it does not matter how
   non-generic the degree is, just that it is non-generic at all.

   The return value is the number of generators that raise var to
   exp. If there are no non-generic pairs (i.e. the ideal is generic)
   then the return value is zero and var=exp=0.

   This method is not const since it may re-arrange the generators for
   efficiency.
  */
  size_t getTypicalNonGenericExponent(size_t& var, Exponent& exp);

  /** Sets var and exp such that var^exp is some non-generic
   degree. Returns true if there are any non-generic degrees and
   otherwise returns false.

   This method is not const since it may re-arrange the generators for
   efficiency.
  */
  bool getNonGenericExponent(size_t& var, Exponent& exp);

  // returns the first generator that var divides or end() if no such
  // generator exists.
  const_iterator getMultiple(size_t var) const;

  bool operator==(const Ideal& ideal) const;

  void print(FILE* file) const;
  void print(ostream& out) const;

  // *** Mutators

  // Insert generators into the ideal.
  void insert(const Exponent* term);
  void insert(const Ideal& ideal);
  void insert(size_t var, Exponent e);

  // This is equivalent to calling insert and then minimize.
  void insertReminimize(const Exponent* term);
  void insertReminimize(size_t var, Exponent e);

  // Remove non-redundant generators.
  void minimize();

  // Sorts the generators in the specified order
  void sortReverseLex(); // reverse lexicographic order
  void sortLex(); // lexicographic order from left

  // Sort the generators in ascending order according to the exponent of var.
  void singleDegreeSort(size_t var);

  // Replace each generator g by g * term.
  void product(const Exponent* term);

  // Replace each generator g by g : by. The second overload has by
  // equal to var raised to e.
  void colon(const Exponent* by);
  void colon(size_t var, Exponent e);

  // Equivalent to calling colon(by) and then minimize. The second
  // overload has by equal to var raised to e. Returns true if the support
  // of any generator was changed.
  bool colonReminimize(const Exponent* colon);
  bool colonReminimize(size_t var, Exponent e);

  // Swaps it and the last element, and then removes the last element,
  // which is the element originally pointed to by it.
  void remove(const_iterator it);

  // Removes those generators that are multiples of term. The second
  // overload has term equal to var raised to e.
  void removeMultiples(const Exponent* term);
  void removeMultiples(size_t var, Exponent e);

  // Insert those generators of ideal that are not multiples of
  // term. The second overload has term equal to var raised to e.
  void insertNonMultiples(const Exponent* term, const Ideal& ideal);
  void insertNonMultiples(size_t var, Exponent e, const Ideal& ideal);

  // Removes those generators that are strict multiples of term.
  void removeStrictMultiples(const Exponent* term);

  // Remove duplicate generators.
  void removeDuplicates();

  // Removes all generators, and optionally sets the number of variables.
  void clear();
  void clearAndSetVarCount(size_t varCount);

  /** Replaces the exponents from zeroExponents with zero and does not
      remove any non-minimal generators this may produce. If a
      generator raises var to precisely the power t, and zeroExponents
      also raises var to precisely the power t, then that generator is
      changed to instead raise var to the power zero. */
  void mapExponentsToZeroNoMinimize(const Term& zeroExponents);

  /** Replaces all generators with their support and does not remove
      any non-minimal generators this may produce.
  */
  void takeRadicalNoMinimize();

  Ideal& operator=(const Ideal& ideal);

  void swap(Ideal& ideal);

  /** Removes those generators m such that pred(m) evaluates to
   true. Returns true if any generators were removed.
  */
  template<class Predicate>
    bool removeIf(Predicate pred);

  /** Ideal caches memory allocated with new internally and reuses it
   to avoid calling new all the time. Call this method to release
   the cache.
  */
  static void clearStaticCache();

 protected:
  class ExponentAllocator {
  public:
    ExponentAllocator(size_t varCount);
    ~ExponentAllocator();

    Exponent* allocate();
    void reset(size_t newVarCount);

    void swap(ExponentAllocator& allocator);

  private:
    ExponentAllocator(const ExponentAllocator&);
    ExponentAllocator& operator=(const ExponentAllocator&);

    bool useSingleChunking() const;

    size_t _varCount;

    Exponent* _chunk;
    Exponent* _chunkIterator;
    Exponent* _chunkEnd;

    vector<Exponent*> _chunks;
  };

  size_t _varCount;
  vector<Exponent*> _terms;
  ExponentAllocator _allocator;
};

template<class Predicate>
inline bool Ideal::removeIf(Predicate pred) {
  iterator newEnd = std::remove_if(_terms.begin(), _terms.end(), pred);

  if (newEnd != _terms.end()) {
    _terms.erase(newEnd, _terms.end());
    return true;
  } else
    return false;
}

inline ostream& operator<<(ostream& out, const Ideal& ideal) {
  ideal.print(out);
  return out;
}

#endif

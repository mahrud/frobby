/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

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
#include "BigattiHilbertAlgorithm.h"

#include "Ideal.h"
#include "CoefTermConsumer.h"
#include "BigattiState.h"

BigattiHilbertAlgorithm::
BigattiHilbertAlgorithm(const Ideal& ideal, CoefTermConsumer& consumer):
 _consumer(&consumer),
 _baseCase(ideal.getVarCount()),
 _useGenericBaseCase(true),
 _useSimplification(true),
 _pivot(0),
 _printDebug(false),
 _printStatistics(false),
 _doCanonicalOutput(false) {

   ASSERT(ideal.isMinimallyGenerated());
  _varCount = ideal.getVarCount();
  _tmp_simplify_gcd.reset(_varCount);

  _tasks.addTask(new BigattiState(this, ideal, Term(_varCount)));
}

void BigattiHilbertAlgorithm::setPrintStatistics(bool value) {
  _printStatistics = value;
}

void BigattiHilbertAlgorithm::setPrintDebug(bool value) {
  _printDebug = value;
  _baseCase.setPrintDebug(value);
}

void BigattiHilbertAlgorithm::setUseGenericBaseCase(bool value) {
  _useGenericBaseCase = value;
}

void BigattiHilbertAlgorithm::setUseSimplification(bool value) {
  _useSimplification = value;
}

void BigattiHilbertAlgorithm::setDoCanonicalOutput(bool value) {
  _doCanonicalOutput = value;
}

void BigattiHilbertAlgorithm::
setPivotStrategy(auto_ptr<BigattiPivotStrategy> pivot) {
  _pivot = pivot;
}

void BigattiHilbertAlgorithm::run() {
  if (_pivot.get() == 0)
	_pivot = BigattiPivotStrategy::createStrategy("median");

  _tasks.runTasks();
  _baseCase.feedOutputTo(*_consumer, _doCanonicalOutput);

  if (_printStatistics) {
	fputs("*** Statistics for run of Bigatti algorithm ***\n", stderr);
	fprintf(stderr, " %u states processed.\n", _tasks.getTotalTasksEver());
	fprintf(stderr, " %u base cases.\n", _baseCase.getTotalBaseCasesEver());
	fprintf(stderr, " %u terms output.\n", _baseCase.getTotalTermsOutputEver());
	fprintf(stderr, " %u terms in final output.\n", _baseCase.getTotalTermsInOutput());
  }
}

void BigattiHilbertAlgorithm::processState(auto_ptr<BigattiState> state) {
  if (_useSimplification)
	simplify(*state);

  if (_printDebug) {
	fputs("Debug: Processing state.\n", stderr);
	state->print(stderr);
  }

  bool isBaseCase = _useGenericBaseCase ?
	_baseCase.genericBaseCase(*state) :
	_baseCase.baseCase(*state);
  if (isBaseCase) {
	freeState(state);
	return;
  }

  const Term& pivot = _pivot->getPivot(*state);
  if (_printDebug) {
	fputs("Debug: Performing pivot split on ", stderr);
	pivot.print(stderr);
	fputs(".\n", stderr);
  }
  ASSERT(!pivot.isIdentity());
  ASSERT(!state->getIdeal().contains(pivot));

  auto_ptr<BigattiState> colonState(_stateCache.newObjectCopy(*state));
  colonState->colonStep(pivot);
  _tasks.addTask(colonState.release());

  state->addStep(pivot);
  _tasks.addTask(state.release());
}

void BigattiHilbertAlgorithm::simplify(BigattiState& state) {
  Term& gcd = _tmp_simplify_gcd;
  ASSERT(gcd.getVarCount() == _varCount);

  state.getIdeal().getGcd(gcd);
  if (!gcd.isIdentity()) {
    // Do colon and output multiply-gcd*multiply.
    _baseCase.output(true, state.getMultiply());
	state.colonStep(gcd);
    _baseCase.output(false, state.getMultiply());
  }

  IF_DEBUG(state.getIdeal().getGcd(gcd));
  ASSERT(gcd.isIdentity());
}

void BigattiHilbertAlgorithm::freeState(auto_ptr<BigattiState> state) {
  state->getIdeal().clear(); // To preserve memory
  _stateCache.freeObject(state);
}

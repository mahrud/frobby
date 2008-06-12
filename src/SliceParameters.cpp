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
#include "SliceParameters.h"

#include "SliceFacade.h"

SliceParameters::SliceParameters(bool exposeBoundParam):
  _exposeBoundParam(exposeBoundParam),

  _printDebug
  ("debug",
   "Print what the algorithm does at each step.",
   false),

  _printStatistics
  ("stats",
   "Print statistics on what the algorithm did.",
   false),

  _useBound
  ("bound",
   "Use the bound optimization when solving an IDP.",
   true),

  _useIndependence
  ("independence",
   "Perform independence splits when possible.",
   true),
  
  _minimal
  ("minimal",
   "Specifies that the input ideal is minimally generated by the given\n"
   "generators. Turning this on can improve performance, but if it is not\n"
   "true, Frobby may go into an infinite loop or produce incorrect results.",
   false),

  _canonical
  ("canon",
   "Sort the output, including the variables, to get a canonical\n"
   "representation. This requires storing the entire output in memory, which\n"
   "can increase run time modestly and increase memory consumption greatly.",
   false),

  _split
  ("split",
   "The split selection strategy to use. Options are maxlabel, minlabel,\n"
   "varlabel, minimum, median, maximum, mingen, indep and gcd. Frobenius\n"
   "computations support the specialized strategy frob as well.",
   "median") {
  addParameter(&_minimal);
  addParameter(&_split);
  //addParameter(&_printStatistics); // TODO: reimplement
  addParameter(&_useIndependence);
  addParameter(&_printDebug);

  if (_exposeBoundParam)
	addParameter(&_useBound);
  else
	addParameter(&_canonical);
}

void SliceParameters::setUseIndependence(bool value) {
  _useIndependence = value;
}

void SliceParameters::setSplit(const string& split) {
  _split = split;
}

bool SliceParameters::getUseBound() const {
  return _useBound;
}

void SliceParameters::validateSplit(bool allowLabel,
											   bool allowFrob) {
  const SplitStrategy* split =
	SplitStrategy::getStrategy(_split.getValue().c_str());

  if (split == 0) {
	fprintf(stderr, "ERROR: Unknown split strategy \"%s\".\n",
			_split.getValue().c_str());
	exit(1);
  }

  if (split->isFrobeniusSplit() && !allowFrob) {
	fputs("ERROR: Frobenius split strategy is not appropriate\n"
		  "in this context.\n", stderr);
	exit(1);
  }

  if (!allowLabel && split->isLabelSplit()) {
	fputs("ERROR: Label split strategy is not appropriate\n"
		  "in this context.\n", stderr);
	exit(1);
  }
}

void SliceParameters::apply(SliceFacade& facade) const {
  const SplitStrategy* split =
	SplitStrategy::getStrategy(_split.getValue().c_str());

  facade.setPrintDebug(_printDebug);
  facade.setPrintStatistics(_printStatistics);
  facade.setUseIndependence(_useIndependence);
  facade.setIsMinimallyGenerated(_minimal);
  facade.setSplitStrategy(split);
  if (_canonical)
	facade.setCanonicalOutput();
}

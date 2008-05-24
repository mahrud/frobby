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
#include "HilbertAction.h"

#include "BigIdeal.h"
#include "IOFacade.h"
#include "IdealFacade.h"
#include "Scanner.h"

HilbertAction::HilbertAction():
  _io(IOParameters::InputOnly),

  _oformat
  ("oformat",
   "The output format. The supported formats are m2 and null.",
   "m2"),

  _canonicalize
  ("canon",
   "Collect and sort terms to get a canonical representation.",
   false) {
}

const char* HilbertAction::getName() const {
  return "hilbert";
}

const char* HilbertAction::getShortDescription() const {
  return "Compute the Hilbert-Poincare series of the input ideal.";
}

const char* HilbertAction::getDescription() const {
  return getShortDescription();
}

Action* HilbertAction::createNew() const {
  return new HilbertAction();
}

void HilbertAction::obtainParameters(vector<Parameter*>& parameters) {
  _io.obtainParameters(parameters);
  parameters.push_back(&_oformat);
  parameters.push_back(&_canonicalize);
  Action::obtainParameters(parameters);
}

void HilbertAction::perform() {
  BigIdeal ideal;

  FILE* output;
  if (_oformat.getValue() == "m2") {
	output = stdout;
  } else if (_oformat.getValue() == "null") {
	output = 0;
  } else {
	fprintf(stderr, "ERROR: Unknown output format \"%s\".",
			_oformat.getValue().c_str());
	exit(1);
  }

  {
	Scanner in(_io.getInputFormat(), stdin);
	_io.autoDetectInputFormat(in);
	_io.validateFormats();

	IOFacade facade(_printActions);

	facade.readIdeal(in, ideal);
  }

  IdealFacade idealFacade(_printActions);
  idealFacade.printHilbertSeries(ideal, _canonicalize, output);
}
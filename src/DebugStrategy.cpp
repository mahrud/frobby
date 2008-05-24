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
#include "DebugStrategy.h"

DebugStrategy::DebugStrategy(SliceStrategy* strategy, FILE* out):
  _strategy(strategy),
  _out(out) {
  ASSERT(strategy != 0);
  fputs("DEBUG: Starting slice computation.\n", _out);
}

DebugStrategy::~DebugStrategy() {
  delete _strategy;
  fputs("DEBUG: Slice computation done.\n", _out);
  fflush(_out);
}

Slice* DebugStrategy::setupInitialSlice(const Ideal& ideal) {
  fputs("Constructing initial slice.\n", _out);
  return _strategy->setupInitialSlice(ideal);
}

void DebugStrategy::split(Slice* slice,
						  SliceEvent*& leftEvent, Slice*& leftSlice,
						  SliceEvent*& rightEvent, Slice*& rightSlice) {
  fputs("DEBUG: Starting split of slice.\n", _out);
  _strategy->split(slice, leftEvent, leftSlice, rightEvent, rightSlice);
  fputs("DEBUG: Done with split of slice.\n", _out);
}

void DebugStrategy::freeSlice(Slice* slice) {
  fputs("DEBUG: Freeing slice.\n", _out);
  _strategy->freeSlice(slice);
}
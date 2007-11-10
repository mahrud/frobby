#ifndef FPLLL_IO_GUARD
#define FPLLL_IO_GUARD

class BigIdeal;

namespace fplll {
  void writeLatticeBasis(ostream& out, const BigIdeal& basis);

  void readLatticeBasis(istream& in, BigIdeal& basis);
}

#endif

#include "stdinc.h"
#include "Strategy.h"

Strategy::~Strategy() {
}

void Strategy::startingPartitioning(const Term& b,
				    const Partition& partition,
				    const TermTree& tree) {
  ASSERT(false);
}
    
void Strategy::doingPartitionSet(int position,
				 const Term& b,
				 const Term& compressedB,
				 const Partition& partition,
				 vector<Exponent> compressor,
				 const TermTree& tree) {
  ASSERT(false);
}

void Strategy::doneDoingPartitionSet(int position,
				     const vector<Exponent>& compressor) {
  string name;
  getName(name);
  cerr << "ERROR: doneDoingPartitionSet not implemented in " << name << "." << endl;
  ASSERT(false);
}

void Strategy::endingPartitioning(int position,
				  const Term& b) {
  string name;
  getName(name);
  cerr << "ERROR: endingPartitioning not implemented in " << name << "." << endl;
  ASSERT(false);
}

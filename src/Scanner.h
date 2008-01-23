#ifndef SCANNER_GUARD
#define SCANNER_GUARD

class VarNames;

class Scanner {
public:
  Scanner(FILE* in);
  ~Scanner();

  bool match(char c);
  bool matchEOF();

  void expect(char expected);
  void expect(const char* str);
  void expect(const string& str);
  void expectEOF();

  void readInteger(mpz_class& integer);
  void readInteger(unsigned int& i);
  void readIntegerAndNegativeAsZero(mpz_class& integer);

  // The returned valid is only valid until the next method on this
  // object gets called.
  const char* readIdentifier();
  size_t readVariable(const VarNames& names);

  bool peekIdentifier();

  unsigned int getLineNumber() const;

  // Writes "ERROR (line ?):" to standard error
  void printError();

private:
  // returns size
  size_t readIntegerString();
  void parseInteger(mpz_class& integer, size_t size);

  int getChar();

  int peek();

  void error(const string& expected);

  void eatWhite();

  void growTmpString();

  mpz_class _integer;
  FILE* _in;
  unsigned long _lineNumber;
  int _char; // next character on stream

  char* _tmpString;
  size_t _tmpStringCapacity;
};

#endif
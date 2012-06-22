// Testing LLVM IR names for different C++ kinds of variable and
// function.

// TODO: add a test for a variable with external C linkage

// Global with external linkage, initialized to 0
int I01;

// Declaration only, definition in another TU
extern int I02;

// Constant global variable, has internal linkage
const int I03 = 3;

// Constant, external linkage forced with keyword 'extern'
extern const int I04 = 4;

// Pointer to constant
const int *P01 = &I03;

// Constant pointer to constant
const int *const P02 = &I03;

// Reference to constant
const int &R01 = I03;

namespace {                     // Unnamed namespace

  // Global with internal linkage
  int I05;

  // Constant with internal linkage
  const int I06 = 6;
}

struct S01 {

  // Static field
  static int I07;

  // Static constant field
  static const int I08;

  // Instance field
  int I09;

  // Constructor
  S01() : I09(9) {};

  // I10: Function argument
  int F01(int I10) {
    return I01 + I02 + I03 + I04 + I05 + I06 + I07 + I08 + I09 + I10;
  }
};

int S01::I07 = 7;
const int S01::I08 = 8;

int main(int ArgC, char *ArgV[]) {
  I05 = *P01 + *P02 + R01 + 5;
  S01 X;
  return X.F01(10);
}

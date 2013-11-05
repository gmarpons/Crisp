// RUN: %crisp_clang_cc1 -plugin-arg-crisp-clang SomeHICPPrules %s -verify

// TODO: update to new lit variable syntax:
// http://llvm.org/docs/TestingGuide.html#variables-and-substitutions

class B {
public:
  B(); // expected-warning 2 {{HICPP 3.3.13: ctor/dtor 'B' calls (maybe indirectly) virtual method 'func'}} // expected-warning {{HICPP 3.3.13: ctor/dtor 'B' calls (maybe indirectly) virtual method 'func2'}}
  B(B* other_b);
  virtual void func() {} // expected-note 2 {{HICPP 3.3.13: called virtual method 'func' declared here}}
  virtual void func2() {} // expected-note {{HICPP 3.3.13: called virtual method 'func2' declared here}}
  virtual void func3() {}
  void non_virtual() { func(); } // expected-note {{HICPP 3.3.13: method 'non_virtual' calls method 'func' here}}
};

class D : public B {
public:
  D() : B() {}
  D(B* other_b) : B(other_b) {}
  virtual void func() {}
  virtual void func2() {}
  virtual void func3() {}
};

B::B() {
  func();			// expected-note {{HICPP 3.3.13: method 'B' calls method 'func' here}}
  this->func2();		// expected-note {{HICPP 3.3.13: method 'B' calls method 'func2' here}}
  non_virtual();		// expected-note {{HICPP 3.3.13: method 'B' calls method 'non_virtual' here}}
}

B::B(B* other_b) {
  other_b->func3();		// D::func3 called, on already
                                // constructed object
}

int main() {
  D* d1 = new D();
  D* d2 = new D(d1);
  return 0;
}

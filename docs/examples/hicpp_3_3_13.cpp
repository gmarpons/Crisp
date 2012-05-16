class B {
public:
  B();
  B(B* other_b);
  virtual void func() {}
  virtual void func2() {}
  virtual void func3() {}
  void non_virtual() { func(); }
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
  func();			// B::func called, not D::func
  this->func2();		// B::func2 called, not D::func2
  non_virtual();		// B::func called, not D::func
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

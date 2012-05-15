class B {
public:
  B();
  B(B* other_b);
  virtual void func() {}
  virtual void func2() {}
  void non_virtual() { func(); }
};

class D : public B {
public:
  D() : B() {}
  D(B* other_b) : B(other_b) {}
  virtual void func() {}
  virtual void func2() {}
};

B::B() {
  func();			// B::func called, not D::func
  non_virtual();		// B::func called, not D::func
}

B::B(B* other_b) {
  other_b->func2();		// D::func2 called, on already
                                // constructed object
}

int main() {
  D* d1 = new D();
  D* d2 = new D(d1);
  return 0;
}

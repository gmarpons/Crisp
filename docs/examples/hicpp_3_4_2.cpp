class A {
public:
  A() {};

  int* foo(bool b) const {
    int* local = 0;
    if (b) local = m_pa;
    return local;
  }
private:
  int* m_pa;
};

void bar() {
  const A a;
  int* pa = a.foo(true);
  *pa = 10;
};

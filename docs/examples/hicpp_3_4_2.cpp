class A {
public:
  A() {};

  int* fooWrong(bool b) const {
    int* local = 0;
    if (b) local = m_pa;
    return local;
  }

  int* fooWrongWithLoop(int pos) const {
    const int N = 5;
    int* array[N];
    int i = 0;
    while (i < N-1) {
      array[i] = new int;
      i++;
    }
    array[i] = m_pa;
    return array[pos];
  }

  int* fooRight() const {
    int* local = new int;
    *local = 5;
    return local;               // doesn't point to local data
  }

private:
  int* m_pa;
};

void bar() {
  const A a;
  int* pa = a.fooWrong(true);
  *pa = 10;
  pa = a.fooWrongWithLoop(0);
  pa = a.fooRight();
};

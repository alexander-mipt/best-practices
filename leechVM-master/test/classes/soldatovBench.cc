#include <iostream>
#include <memory>
#include <vector>

//F*ck smart pointers, I use pure operator new
//I don't care about memory leaks
//Cry about it, lol
class Bar
{
public:
  int a;
  Bar(int a_) : a(a_) {}
};

class Foo
{
  int x;
  Bar* y;

public:
  Foo(int x_) : x(x_), y(nullptr) {}
  Bar* getY() const { return y; }
  void setY(Bar* y_) { y = y_;}
};

void dump(const std::vector<Foo*>& fooArr, long M)
{
  for (auto i = 0; i < M; ++i)
  {
    auto& f = fooArr[i];
    if (f == nullptr)
    {
      std::cout << "Foo:null" << std::endl;
    }
    else
    {
      auto&& b = f->getY();
      if (b == nullptr)
      {
        std::cout << "Foo.Bar:null" << std::endl;
      }
      else
      {
        std::cout << b->a << std::endl;
      }
    }
  }
}

void foo(long N, long M)
{
  std::vector<Foo*> fooArr(M, nullptr);
  [[maybe_unused]] Foo* outer = nullptr;


  for (auto i = 0; i < N; ++i)
  {

    Foo* o1 = new Foo(i);
    if(!(i % 3))
      fooArr[i % (M - 1)] = o1;

    Bar* o2= new Bar(i);
    if(!(i % 5))
      o1->setY(o2);

    outer = o1;
  }

  dump(fooArr, M);

}

int main()
{
  long N = 4000000;
  long M = 1000;
  foo(N,M);
  return 0;
}

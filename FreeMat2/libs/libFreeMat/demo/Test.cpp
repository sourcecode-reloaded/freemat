#include <iostream>

class Shape {
public:
  Shape() {}
  virtual ~Shape() {}
};

class Box : public Shape {
public:
  Box() {}
  virtual ~Box() {}
};

class Circle : public Shape {
public:
  Circle() {}
  virtual ~Circle() {}
};

Box squareit(const Shape &t) {
  std::cout << "input " << typeid(t).name() << "\n";
  //  if (typeid(t) == typeid(const Circle &)) {
  return Box();
  //  }
}

void square(Shape &t) {
  t = Box();
}

int main(int argc, char *argv[]) {
  Circle c;
  Box b;
  std::cout << " b type " << typeid(b).name() << "\n";
  std::cout << " c type " << typeid(c).name() << "\n";
  Shape & f = c;
  std::cout << " f type " << typeid(f).name() << "\n";
  const Shape & g = squareit(c);
  std::cout << " square " << typeid(g).name() << "\n";
  square(c);
  std::cout << " c type " << typeid(c).name() << "\n";
  return 0;
}

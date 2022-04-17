#pragma once
#include <iostream>
/**
 *  Логирующий класс для контроля создания, копирования, перемещения, присваивания, удаления */
class A {
public:
  int id;
  A(int id) :id(id) {
    std::cout << "A(" << id << "); ";
  }
  ~A() {
    std::cout << "~A(" << id << "); ";
    id = 0;
  }
  A(const A& other) {
    id = other.id;
    std::cout << "A-cc(" << id << "); ";
  }
  A(A&& other) noexcept {
    id = other.id;
    other.id = -id;
    std::cout << "A-mc(" << id << "); ";
  }
  A& operator=(const A& other) {
    std::cout << "A=(" << id << "="<<other.id<<"); ";
    id = other.id;
    return *this;
  }
  A& operator=(A&& other) {
    std::cout << "A<=(" << id << "="<<other.id<<"); ";
    id = other.id;
    other.id = -id;
    return *this;
  }

  bool operator==(const A& rv) const {
    return rv.id == id;
  }
};

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
};

/**
 * @brief Функция для  проверки п выводу в std::out 
 * работы массива с элементами
 */
template< template<class T> class TARR>
void test_array_internal() {
    constexpr auto print_array_of_A = [](const TARR<A>& arr) {
        std::cout << "\n[";
        for (int i = 0; i < arr.count(); i++) {
            std::cout << arr.get(i).id << " ";
        }
        std::cout << "]" << std::endl;
    };
    A a2(2), a3(3), a4(4);
    std::cout << "\nCreating array" << std::endl;
    TARR<A> arr;
    arr.count();

    std::cout << "Add rvalue A1" << std::endl;
    arr.add(A(1));
    print_array_of_A(arr);

    std::cout << "Add A2" << std::endl;
    arr.add(a2);
    print_array_of_A(arr);

    std::cout << "Insert A3 at 1" << std::endl;
    arr.insert(a3, 1);
    print_array_of_A(arr);

    std::cout << "Remove A3 at 1" << std::endl;
    arr.remove(1);
    print_array_of_A(arr);

    std::cout << "Insert A4 at 0" << std::endl;
    arr.insert(a4, 0);
    print_array_of_A(arr);

    std::cout << "Insert A5 at 0" << std::endl;
    arr.insert(A(5), 0);
    print_array_of_A(arr);

    std::cout << "Destroying array" << std::endl;
}
#pragma once

/**
 *  Логирующий класс для контроля создания, копирования, перемещения, присваивания, удаления */
class B {
    static inline int constuctor_count_ = 0;
    static inline int destructor_count_ = 0;
    static inline int assign_count_ = 0;
    static inline int equation_count_ = 0;
public:
    int id;
    static auto constuctor_count() { return constuctor_count_; }
    static auto destructor_count() { return destructor_count_; }
    static auto assign_count() { return assign_count_; }
    static auto equation_count() { return equation_count_; }
    static void reset() {
        constuctor_count_ = 0;
        destructor_count_ = 0;
        assign_count_ = 0;
        equation_count_ = 0;
    }

    explicit B(int id) :id(id) {
        constuctor_count_++;
    }
    ~B() {
        destructor_count_++;
    }
    B(const B& other) {
        id = other.id; constuctor_count_++;
    }
    B(B&& other) noexcept {
        id = other.id; constuctor_count_++;
    }
    B& operator=(const B& other) {
        assign_count_++;
        id = other.id;
        return *this;
    }
    B& operator=(B&& other) {
        assign_count_++;
        id = other.id;
        return *this;
    }

    bool operator==(const B& rv) const {
        return rv.id == id;
        equation_count_++;
    }
};

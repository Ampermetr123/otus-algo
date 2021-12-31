#pragma once
#include <memory>
#include <functional>
#include <tuple>


/**
 * @brief Splay дерево поиска
 * @tparam T - тип хранимых элементов
 */
template <typename T>
class SplayTree {
private:
    struct Node;
    using node_uptr = std::unique_ptr< Node >;
    struct Node {
        node_uptr left;
        node_uptr right;
        T val;
        Node(const T& x) :val(x) {}
    };

    node_uptr root;
    void right_rotate(node_uptr& it);
    void left_rotate(node_uptr& it);
    void splay(node_uptr& it, const T& x);
    void walk(const std::function<void(const T&, int)>& f, const Node* it, int depth);

public:
    SplayTree() = default;
    ~SplayTree() = default;
    SplayTree(const SplayTree& other) = delete;
    SplayTree(SplayTree&& other) = delete;

    void insert(const T& x);
    void remove(const T& x);
    bool search(const T& x);
    void for_each(const std::function<void(const T&, int)>& f);
};



/***************    PUBLIC FUNCTIONS     **************************************/

/**
*  Вставка элемента x. Дубликаты игнорируются, но "встряхивают" дерево
*  В некоторых источниках (https://people.eecs.berkeley.edu/~jrs/61b/lec/36)
*  "insert() begins by inserting the new entry (k, v), just like in an ordinary bin
*  Then, it splays the new node to the root.
*  В этой реализации сначала поднимается в корень (splay) подходящий для вставки нового значения элемент, 
*  а затем задается новый корень с узлом x. Так быстрее, но в некоторых ситуациях получается другое дерево
*/

template<typename T>
void SplayTree<T>::insert(const T& x) {

    // случай пустого дерева
    if (root == nullptr) {
        root.reset(new Node(x));
        return;
    }

    // поднимаем узел для вставки в корень
    splay(root, x);

    // случай дубликата 
    if (root->val == x) {
        return;
    }
    auto node = node_uptr(new Node(x));
    if (x < root->val) {
        // cделать root правым потомком
        node->right = std::move(root);
        node->left = std::move(node->right->left);
        root = std::move(node);
    }
    else {
        // cделать root левым потомком
        node->left = std::move(root);
        node->right = std::move(node->left->right);
        root = std::move(node);
    }
}


/**
 * Удаление элемента X. 
 */
template<typename T>
void SplayTree<T>::remove(const T& x) {
    splay(root, x);

    if (root == nullptr || root->val != x) {
        return;
    }
    else if (root->left == nullptr && root->right == nullptr) {
        root.reset();
    }
    else if (root->left == nullptr) {
        root = std::move(root->right);
    }
    else if (root->right == nullptr) {
        root = std::move(root->left);
    }
    else if (root->right->val > root->left->val) {
        // найти самый большой лист,  поменять местами c корнем и удалить этот лист
        Node* p = root.get();
        while (p->right->right != nullptr) {
            p = p->right.get();
        }
        auto& list = p->right;
        std::swap(list->val, root->val);
        p->right.reset();
    }
};


/**
 * Поиск узла со значением x
 */
template<typename T>
bool SplayTree<T>::search(const T& x) {
    splay(root, x);
    if (root == nullptr || !(root->val == x)) {
        return false;
    }
    return true;
}


/**
 * Проход по дереву в порядке возрастания с применением функции f
 * f принимает два параметра - значение элемента узла и глубину его расположения в дереве
 */
template<typename T>
void SplayTree<T>::for_each(const std::function<void(const T&, int)>& f) {
    if (root) {
        walk(f, root.get(), 0);
    }
}


/***************    PRIVATE FUNCTIONS     **************************************/

/** Поворот дерева с конем it направо. 
 *  В результате it станет корнем повернутого дерева*/
template<typename T>
void SplayTree<T>::right_rotate(node_uptr& it) {
    assert(it != nullptr);
    node_uptr top_right_left = std::move(it->left->right);
    node_uptr top_left = std::move(it->left->left);
    node_uptr top = std::move(it->left);
    top->right = std::move(it);
    top->right->left = std::move(top_right_left);
    top->left = std::move(top_left);
    it = std::move(top);

}


/** Поворот дерева с корнем it налево. 
 *  В результате it станет корнеовернутого дерева*/    
template<typename T>
void SplayTree<T>::left_rotate(node_uptr& it) {
    assert(it != nullptr);
    node_uptr top_left_right = std::move(it->right->left); // move B
    node_uptr top_right = std::move(it->right->right);
    node_uptr top = std::move(it->right);
    top->left = std::move(it);
    top->left->right = std::move(top_left_right);
    top->right = std::move(top_right);
    it = std::move(top);
}


template<typename T>
void SplayTree<T>::splay(typename SplayTree<T>::node_uptr& it, const T& x) {

    if (it == nullptr || it->val == x) {
        return;
    }

    if (x < it->val) {
        if (it->left == nullptr) { // ключ не найден
            return;
        }
        if (x < it->left->val) { // Zig-Zig (do Right G, then Right G rotation)
            splay(it->left->left, x);
            right_rotate(it); // первый поворот
        }
        else if (x > it->left->val) { // Zag-Zig (do Left P , then Right G rotation)
            splay(it->left->right, x);
            if (it->left->right != nullptr) {
                left_rotate(it->left); // первый поворот налево
            }
        }
        if (it->left == nullptr) {
            return;
        }
        else {
            right_rotate(it);
            return;
        }
    }
    else { // if ( x > it->val)

        if (it->right == nullptr) { // ключ не найден
            return;
        }
        if (x > it->right->val) { // Zag-Zag (do Left - G , then Left  - G rotation)
            splay(it->right->right, x);
            left_rotate(it); // первый поворот
        }
        else if (x < it->right->val) { // Zig-Zag (do Right P, then Left G rotation)
            splay(it->right->left, x);
            if (it->right->left != nullptr) {
                right_rotate(it->right); // первый поворот направо
            }
        }
        if (it->right == nullptr) {
            return;
        }
        else {
            left_rotate(it);
            return;
        }
    }
}


/**
 * Рекурсивный проход вглубину по всем элеметам дерева и применением функции f. 
 * Элементы выбираются по возрастанию.
 * */
template<typename T>
void SplayTree<T>::walk(const std::function<void(const T&, int)>& f, const Node* it, int depth) {
    if (it == nullptr) {
        return;
    }
    walk(f, it->left.get(), depth + 1);
    f(it->val, depth);
    walk(f, it->right.get(), depth + 1);
}
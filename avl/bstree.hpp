#pragma once
#include <memory>
#include <functional>

/**
 * @brief Бинарное дерево поиска
 * @tparam T - тип хранимых элементов
 */
template <typename T>
class BSTree {
private:
    struct Node;
    using node_sptr = std::shared_ptr<Node>;
    using node_wptr = std::weak_ptr<Node>;

    struct Node {
        node_sptr left;
        node_sptr right;
        node_wptr parent;
        T val;
        Node(const T& x) :val(x) {}
    };

    Node* search_node_ptr(const T& x);
    node_sptr search_node_sptr(const T& x);
    void remove_node(node_sptr it);
    void walk(std::function<void(node_sptr)>& f, const node_sptr& p);

    node_sptr root;

public:
    BSTree() = default;
    ~BSTree() = default;
    BSTree(const BSTree& other) = delete;
    BSTree(BSTree&& other) = delete;

    void insert(const T& x, std::function<void(BSTree<T>::Node*) > Deleter = std::default_delete<BSTree<T>::Node>());
    void remove(const T& x);
    bool search(const T& x);
    void for_each(std::function<void(node_sptr)> f);
};



/***************    PUBLIC FUNCTIONS     **************************************/

/**
 * Вставка 
 * @param x - элемент для вставки
 * @param Deleter - алтернативный объект-удалитель Node из памяти (передается в shared_ptr)
 */
template<typename T>
void BSTree<T>::insert(const T& x, std::function<void(BSTree<T>::Node*) > Deleter) {
    auto it = search_node_sptr(x);
    if (it == nullptr) {
        root.reset(new Node(x), Deleter);
    }
    else if (it->val == x) {
        it->val = x; // игнорируем вставку дубликата 
    }
    else if (x > it->val) {
        it->right.reset(new Node(x), Deleter);
        it->right->parent = it;
    }
    else {
        it->left.reset(new Node(x), Deleter);
        it->left->parent = it;
    }
}


template<typename T>
void BSTree<T>::remove(const T& x) {
    auto pnode = search_node_sptr(x);
    if (pnode && pnode->val == x)
        remove_node(pnode);
};


template<typename T>
bool BSTree<T>::search(const T& x) {
    auto pnode = search_node_ptr(x);
    return  (pnode && pnode->val == x);
}

/**
 * Проход по дереву в порядке возрастания с применением функции f
 */
template<typename T>
void BSTree<T>::for_each(std::function<void(node_sptr)> f) {
    walk(f, root);
}

/***************    PRIVATE FUNCTIONS     **************************************/

/**
 *  Возвращает прямой укзатель на узел с элементом x, 
 *  либо на узел, подходящий для вставки элемента x
 *  Возвращает nullptr, если дерево пустое
 */
template<typename T>
typename BSTree<T>::Node* BSTree<T>::search_node_ptr(const T& x) {
    if (root == nullptr) {
        return nullptr;
    }
    auto it = root.get();
    Node* it_prev = it;
    do {
        it_prev = it;
        if (x == it->val) {
            break;
        }
        while (x > it->val && it->right != nullptr) {
            it = it->right.get();
        }
        while (x < it->val && it->left != nullptr) {
            it = it->left.get();
        }
    } while (it != it_prev);
    return it;
}


/**
 *  Возвращает умный укзатель на узел с элементом x, 
 *  либо подходящим для вставки элемента x
 */
template<typename T>
typename BSTree<T>::node_sptr BSTree<T>::search_node_sptr(const T& x) {
    auto it = search_node_ptr(x);
    if (it != nullptr) {
        // Восстанавливаем и возвращаем умный указатель через ссылку на parent
        if (auto parent = it->parent.lock(); parent != nullptr) {
            if (parent->right.get() == it) {
                return parent->right;
            }
            else {
                return parent->left;
            }
        }
    }
    return root;
}


/**
 * Удаление узла it из дерева
 */
template<typename T>
void BSTree<T>::remove_node(node_sptr it) {
    if (it == nullptr) {
        return;
    }
    else if (auto p = it->right; p != nullptr) {
        while (p->left != nullptr) {
            p = p->left;
        }
        std::swap(it->val, p->val);
        remove_node(p);
    }
    else if (p = it->left; p != nullptr) {
        while (p->right != nullptr) {
            p = p->right;
        }
        std::swap(it->val, p->val);
        remove_node(p);
    }
    else {
        // удаление листа
        if (auto parent = it->parent.lock(); parent != nullptr) {
            if (parent->right == it) {
                parent->right.reset();
            }
            else {
                parent->left.reset();
            }
        }
        else {
            root.reset(); // удаление последнего элемента в дереве
        }
    }
}


/**
 * Рекурсивный проход вглубину по всем элементам дерева и применением функции f. 
 * Элементы выбираются по возрастанию.
 * */
template<typename T>
void BSTree<T>::walk(std::function<void(node_sptr)>& f, const node_sptr& p) {
    if (p == nullptr) {
        return;
    }
    walk(f, p->left);
    f(p);
    walk(f, p->right);
}
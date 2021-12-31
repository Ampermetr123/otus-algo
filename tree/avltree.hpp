#pragma once
#include "bstree.hpp"
#include <iostream>
#include <assert.h>

template<typename T>
class AVLTree {
private:
    struct Node;
    using node_sptr = std::shared_ptr<Node>;
    using node_wptr = std::weak_ptr<Node>;

    struct Node {
        node_sptr left;
        node_sptr right;
        node_wptr parent;
        size_t height;
        T val;
        Node(const T& x, size_t heigth = 1) :val(x), height(heigth) {}
    };

    node_sptr root;

    std::pair<size_t, size_t> child_heights(const node_sptr& it) {
        return { (it->left == nullptr) ? 0 : it->left->height ,
                 (it->right == nullptr) ? 0 : it->right->height };
    }

    Node* search_node_ptr(const T& x);
    node_sptr search_node_sptr(const T& x);

    void remove_node(node_sptr it);
    node_sptr update_height_up(node_sptr it);
    void rebalance(node_sptr it);
    void small_left_rotation(node_sptr it);
    void small_right_rotation(node_sptr it);
    void big_left_rotation(node_sptr it);
    void big_right_rotation(node_sptr it);
    void walk(std::function<void(node_sptr)>& f, const node_sptr& p);

public:
    AVLTree() = default;
    ~AVLTree() = default;
    AVLTree(const AVLTree& other) = delete;
    AVLTree(AVLTree&& other) = delete;
    
    void insert(const T& x, std::function<void(Node*) > Deleter = std::default_delete<Node>());
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
void AVLTree<T>::insert(const T& x, std::function<void(Node*) > Deleter) {
    std::shared_ptr<Node> it = search_node_sptr(x);

    if (it == nullptr) { // Первый элемент 
        root.reset(new Node(x), Deleter);
        return;
    }

    if (it->val == x) { // Дубликат
        it->val = x;
        return;
    }

    // Новый элемент
    node_sptr ins_node(new Node(x), Deleter);
    ins_node->parent = it;
    if (x > it->val) {
        it->right = ins_node;
    }
    else {
        it->left = ins_node;
    }

    auto next = update_height_up(it);
    while (next != nullptr) {
        rebalance(next);
        next = update_height_up(next->parent.lock());
    }
}


template<typename T>
void AVLTree<T>::remove(const T& x) {
    auto pnode = search_node_sptr(x);
    if (pnode && pnode->val == x)
        remove_node(pnode);
};


template<typename T>
bool AVLTree<T>::search(const T& x) {
    /* Commented way is to slow !*/
    auto pnode = search_node_ptr(x);
    return  (pnode && pnode->val == x);
}


template<typename T>
void AVLTree<T>::for_each(std::function<void(node_sptr)> f) {
    walk(f, root);
}



/***************     PRIVATE FUNCTIONS    *************************************/

/**
 * Удаление узла it из дерева
 */
template<typename T>
void AVLTree<T>::remove_node(node_sptr it) {
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
        // Листовой элемент
        if (auto parent = it->parent.lock(); parent != nullptr) {
            if (parent->right == it) {
                parent->right.reset();
            }
            else {
                parent->left.reset();
            }

            auto next = update_height_up(parent);
            while (next != nullptr) {
                rebalance(next);
                next = update_height_up(next->parent.lock());
            }
        }
        else {
            root.reset(); // удаление последнего корневого элемента
        }
    }
}



/**
 * @brief Обновление фактора высоты для узла it и всех его предков
 * до первого узла, который требует балансировки.
 * @return nullptr - не требуется балансировка
 * @return укзатель на узел, который требует балансировки
 */
template<typename T>
typename AVLTree<T>::node_sptr AVLTree<T>::update_height_up(node_sptr it) {
    if (it == nullptr) {
        return it;
    }
    auto [lh, rh] = child_heights(it);

    auto h = std::max(rh, lh) + 1;

    // С новой высотой разница высот >1 ?
    if (rh > lh + 1 || lh > rh + 1) {
        it->height = h;
        return it;
    }

    // Высота изменилась - идем выше
    if (h != it->height) {
        it->height = h;
        auto parent = it->parent.lock();
        if (parent) {
            return update_height_up(parent);
        }
        else {
            return nullptr; // achive root, and balance is ok
        }
    }

    // высота не изменилась 
    return nullptr;
}


template<typename T>
void AVLTree<T>::rebalance(node_sptr it) {
    auto [lh, rh] = child_heights(it);
    if (lh > 1 + rh) {
        auto [llh, lrh] = child_heights(it->left);
        if (llh >= lrh) {
            small_right_rotation(it);
        }
        else {
            big_right_rotation(it);
        }
    } else if (rh > lh + 1) {
        auto [rlh, rrh] = child_heights(it->right);
        if (rrh >= rlh) {
            small_left_rotation(it);
        }
        else {
            big_left_rotation(it);
        }
    }
};


template<typename T>
void AVLTree<T>::small_left_rotation(node_sptr it) {
    assert(it->right != nullptr);
    auto top = it->right;
    auto lr = it->right->left;
    top->left = it;

    top->parent = it->parent;
    if (auto it_parent = it->parent.lock(); it_parent != nullptr) {
        if (it_parent->right == it) {
            it_parent->right = top;
        } else {
            it_parent->left = top;
        }
    } else {
        root = top;
    }

    it->parent = top;
    it->right = lr;
    if (lr) {
        lr->parent = it;
    }

    update_height_up(it);
    assert(it->right != it);
    assert(it->left != it);
    assert(top->right != top);
    assert(top->left != top);
};


template<typename T>
void AVLTree<T>::small_right_rotation(node_sptr it) {
    assert(it->left != nullptr);
    auto top = it->left;
    auto rl = it->left->right;

    top->parent = it->parent;
    if (auto it_parent = it->parent.lock(); it_parent != nullptr) {
        if (it_parent->right == it) {
            it_parent->right = top;
        }
        else {
            it_parent->left = top;
        }
    }
    else {
        root = top;
    }

    top->right = it;
    it->parent = top;
    it->left = rl;
    if (rl) {
        rl->parent = it;
    }

    update_height_up(it);
    assert(it->right != it);
    assert(it->left != it);
    assert(top->right != top);
    assert(top->left != top);
};


template<typename T>
void AVLTree<T>::big_left_rotation(node_sptr it) {
    assert(it->right != nullptr && it->right->left != nullptr);
    small_right_rotation(it->right);
    small_left_rotation(it);
}


template<typename T>
void AVLTree<T>::big_right_rotation(node_sptr it) {
    assert(it->left != nullptr && it->left->right != nullptr);
    small_left_rotation(it->left);
    small_right_rotation(it);
}


/**
 *  Возвращает прямой укзатель на узел с элементом x, 
 *  либо на узел, подходящий для вставки элемента x
 *  Возвращает nullptr, если дерево пустое
 */
template<typename T>
typename AVLTree<T>::Node* AVLTree<T>::search_node_ptr(const T& x) {
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
 *  Возвращает умный укзательна узел с элементом x, 
 *  либо подходящим для вставки элемента x
 */
template<typename T>
typename AVLTree<T>::node_sptr AVLTree<T>::search_node_sptr(const T& x) {
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
 * Рекурсивный проход вглубину по всем элеметам дерева и применением функции f. 
 * Элементы выбираются по возрастанию.
 * */
template<typename T>
void AVLTree<T>::walk(std::function<void(node_sptr)>& f, const node_sptr& p) {
    if (p == nullptr) {
        return;
    }
    walk(f, p->left);
    f(p);
    walk(f, p->right);
}
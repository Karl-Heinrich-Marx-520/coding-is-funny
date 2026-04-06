#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // 用于 std::max
#include <functional> // 用于 std::function

// 模板化的AVL树节点结构
template <typename KeyType, typename ValueType>
struct AVLNode {
    KeyType key;
    ValueType value;
    int height;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const KeyType& k, const ValueType& val)
        : key(k), value(val), height(1), left(nullptr), right(nullptr) {}
};

template <typename KeyType, typename ValueType>
int getHeight(AVLNode<KeyType, ValueType>* node) {
    if (node == nullptr)
        return 0;
    return node->height;
}

template <typename KeyType, typename ValueType>
int getBalance(AVLNode<KeyType, ValueType>* node) {
    if (node == nullptr)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

template <typename KeyType, typename ValueType>
AVLNode<KeyType, ValueType>* rightRotate(AVLNode<KeyType, ValueType>* y) {
    AVLNode<KeyType, ValueType>* x = y->left;
    AVLNode<KeyType, ValueType>* T2 = x->right;

    // 执行旋转
    x->right = y;
    y->left = T2;

    // 更新高度
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

    // 返回新的根
    return x;
}

template <typename KeyType, typename ValueType>
AVLNode<KeyType, ValueType>* leftRotate(AVLNode<KeyType, ValueType>* x) {
    AVLNode<KeyType, ValueType>* y = x->right;
    AVLNode<KeyType, ValueType>* T2 = y->left;

    // 执行旋转
    y->left = x;
    x->right = T2;

    // 更新高度
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

    // 返回新的根
    return y;
}


template <typename KeyType, typename ValueType>
AVLNode<KeyType, ValueType>* insertNode(AVLNode<KeyType, ValueType>* node, const KeyType& key, const ValueType& value) {
    // 1. 执行标准的BST插入
    if (node == nullptr)
        return new AVLNode<KeyType, ValueType>(key, value);

    if (key < node->key)
        node->left = insertNode(node->left, key, value);
    else if (key > node->key)
        node->right = insertNode(node->right, key, value);
    else {
        // 如果键已经存在，更新其值
        node->value = value;
        return node;
    }

    // 2. 更新节点高度
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

    // 3. 获取平衡因子
    int balance = getBalance(node);

    // 4. 根据平衡因子进行旋转

    // 左左情况
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // 右右情况
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // 左右情况
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // 右左情况
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

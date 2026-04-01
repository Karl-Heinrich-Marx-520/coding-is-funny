#include <utility>  // 用于std::pair

template <typename Key, typename T>
class MyMap {
private:
    // -------------------------- 私有：树节点定义 --------------------------
    struct TreeNode {
        std::pair<Key, T> data;       // 键值对（内部用非const Key，保证删除时可赋值）
        TreeNode* left = nullptr;     // 左子节点
        TreeNode* right = nullptr;    // 右子节点
        TreeNode* parent = nullptr;   // 父节点

        // 节点构造函数
        TreeNode(const Key& key, const T& value, TreeNode* p = nullptr)
            : data(key, value), parent(p) {}
    };

    TreeNode* root = nullptr;  // 二叉搜索树的根节点

    // -------------------------- 私有：辅助工具函数 --------------------------
    // 递归删除以node为根的子树所有节点
    static void clear(TreeNode* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // 找到以node为根的子树中的最小节点（最左节点）
    static TreeNode* minimum(TreeNode* node) {
        if (node == nullptr) return nullptr;
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    // 找到以node为根的子树中的最大节点（最右节点）
    static TreeNode* maximum(TreeNode* node) {
        if (node == nullptr) return nullptr;
        while (node->right != nullptr) {
            node = node->right;
        }
        return node;
    }

    // 找到node的中序后继（迭代器++的核心逻辑）
    static TreeNode* successor(TreeNode* node) {
        if (node == nullptr) return nullptr;
        
        // 情况1：有右子树，后继是右子树的最小节点
        if (node->right != nullptr) {
            return minimum(node->right);
        }

        // 情况2：无右子树，向上找第一个「左孩子祖先」
        TreeNode* parent = node->parent;
        while (parent != nullptr && node == parent->right) {
            node = parent;
            parent = parent->parent;
        }
        return parent;
    }

public:
    // -------------------------- 构造/析构/拷贝控制 --------------------------
    // 默认构造
    MyMap() = default;

    // 析构：清空整棵树
    ~MyMap() {
        clear(root);
        root = nullptr;
    }

    // 禁止拷贝构造/拷贝赋值（避免浅拷贝问题）
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;

    // 支持移动构造/移动赋值（C++11及以上，高效转移所有权）
    MyMap(MyMap&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }
    MyMap& operator=(MyMap&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    // -------------------------- 核心：增/删/查/清空 --------------------------
    // 插入/更新键值对：key存在则更新value，不存在则插入
    void insert(const Key& key, const T& value) {
        // 树为空，直接创建根节点
        if (root == nullptr) {
            root = new TreeNode(key, value);
            return;
        }

        TreeNode* current = root;
        TreeNode* parent = nullptr;

        // 查找插入位置
        while (current != nullptr) {
            parent = current;
            if (key < current->data.first) {
                current = current->left;
            } else if (key > current->data.first) {
                current = current->right;
            } else {
                // key已存在，更新value
                current->data.second = value;
                return;
            }
        }

        // 插入新节点（挂到父节点的左/右）
        if (key < parent->data.first) {
            parent->left = new TreeNode(key, value, parent);
        } else {
            parent->right = new TreeNode(key, value, parent);
        }
    }

    // 查找key：返回对应节点指针，找不到返回nullptr
    TreeNode* find(const Key& key) const {
        TreeNode* current = root;
        while (current != nullptr) {
            if (key < current->data.first) {
                current = current->left;
            } else if (key > current->data.first) {
                current = current->right;
            } else {
                return current;  // 找到目标节点
            }
        }
        return nullptr;  // 未找到
    }

    // 删除指定key的节点
    void erase(const Key& key) {
        TreeNode* node = find(key);
        if (node == nullptr) return;  // 未找到，直接返回

        // 情况1：节点有2个子节点 → 用中序后继替换数据，转为删除后继
        if (node->left != nullptr && node->right != nullptr) {
            TreeNode* succ = minimum(node->right);  // 找到中序后继
            node->data = succ->data;                 // 用后继数据覆盖当前节点
            node = succ;                             // 转为删除后继节点
        }

        // 情况2：节点有0/1个子节点 → 直接替换父节点的指针
        TreeNode* child = (node->left != nullptr) ? node->left : node->right;

        // 更新子节点的父指针
        if (child != nullptr) {
            child->parent = node->parent;
        }

        // 更新父节点的子指针
        if (node->parent == nullptr) {
            root = child;  // 删除的是根节点，更新根
        } else if (node == node->parent->left) {
            node->parent->left = child;
        } else {
            node->parent->right = child;
        }

        delete node;  // 释放节点内存
    }

    // 清空整棵树
    void clear() {
        clear(root);
        root = nullptr;
    }

    // 判断树是否为空
    bool empty() const noex

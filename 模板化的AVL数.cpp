template <typename KeyType, typename ValueType>
class AVLMap {
private:
    AVLNode<KeyType, ValueType>* root;

    // 中序遍历辅助函数
    void inorderHelper(AVLNode<KeyType, ValueType>* node, std::vector<std::pair<KeyType, ValueType>>& res) const {
        if (node != nullptr) {
            inorderHelper(node->left, res);
            res.emplace_back(node->key, node->value);
            inorderHelper(node->right, res);
        }
    }

public:
    AVLMap() : root(nullptr) {}

    // 插入或更新键值对
    void put(const KeyType& key, const ValueType& value) {
        root = insertNode(root, key, value);
    }

    // 查找值，返回指向值的指针，如果键不存在则返回nullptr
    ValueType* get(const KeyType& key) {
        return searchNode(root, key);
    }

    // 删除键值对
    void remove(const KeyType& key) {
        root = deleteNode(root, key);
    }

    // 中序遍历，返回有序的键值对
    std::vector<std::pair<KeyType, ValueType>> inorderTraversal() const {
        std::vector<std::pair<KeyType, ValueType>> res;
        inorderHelper(root, res);
        return res;
    }

    // 析构函数，释放所有节点的内存
    ~AVLMap() {
        // 使用后序遍历释放节点
        std::function<void(AVLNode<KeyType, ValueType>*)> destroy = [&](AVLNode<KeyType, ValueType>* node) {
            if (node) {
                destroy(node->left);
                destroy(node->right);
                delete node;
            }
        };
        destroy(root);
    }
};

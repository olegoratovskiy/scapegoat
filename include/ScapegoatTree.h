#pragma once

#include <vector>

class ScapegoatTree
{
    struct Node
    {
        int key;
        std::size_t subtree_size = 1;
        Node * left = nullptr;
        Node * right = nullptr;
        Node * parent = nullptr;

        Node() = default;

        Node(int key);

        ~Node();
    };

    Node * m_root = nullptr;
    std::size_t m_tree_size = 0;
    std::size_t m_max_tree_size = 0;
    double m_alpha = 0.75;

    static double log_base_value(double base, double value);

    unsigned h_alpha(std::size_t size) const;

    static Node * search(Node * v, int key);

    static void values_impl(Node * v, std::vector<int> & numbers);

    Node * find_scapegoat(Node * v, unsigned height) const;

    static Node * flatten_tree(Node * v, Node * head);

    void rebuild_tree(Node * scapegoat);

    static Node * build_balanced_tree(Node * v, std::size_t size);

    static Node * insert_impl(Node *& v, Node * parent, int value, unsigned & depth);

    static Node * remove_impl(Node * v, int value);

public:
    ScapegoatTree() = default;

    ScapegoatTree(double alpha);

    bool contains(int value) const;

    bool insert(int value);

    bool remove(int value);

    std::size_t size() const;

    bool empty() const;

    std::vector<int> values() const;

    ~ScapegoatTree();
};

#include "ScapegoatTree.h"

#include <cmath>
#include <stdexcept> // for std::invalid_argument exception

ScapegoatTree::ScapegoatTree(double alpha)
    : m_alpha(alpha)
{
    if (alpha < 0.5 || alpha > 1) {
        throw std::invalid_argument("Can't build a tree with such alpha");
    }
}

bool ScapegoatTree::contains(int value) const
{
    return search(m_root, value) != nullptr;
}

bool ScapegoatTree::insert(int value)
{
    if (contains(value)) {
        return false;
    }
    unsigned depth = 0;
    Node * new_v = insert_impl(m_root, nullptr, value, depth);
    m_max_tree_size = std::max(++m_tree_size, m_max_tree_size);
    if (depth > h_alpha(m_tree_size)) {
        Node * scapegoat = find_scapegoat(new_v, 0);
        rebuild_tree(scapegoat);
    }
    return true;
}

bool ScapegoatTree::remove(int value)
{
    if (!contains(value)) {
        return false;
    }
    m_root = remove_impl(m_root, value);
    if (--m_tree_size < m_alpha * m_max_tree_size) {
        rebuild_tree(m_root);
    }
    return true;
}

std::size_t ScapegoatTree::size() const
{
    return m_tree_size;
}

bool ScapegoatTree::empty() const
{
    return size() == 0;
}

std::vector<int> ScapegoatTree::values() const
{
    std::vector<int> numbers;
    numbers.reserve(m_tree_size);
    values_impl(m_root, numbers);
    return numbers;
}

ScapegoatTree::~ScapegoatTree()
{
    delete m_root;
}

unsigned ScapegoatTree::h_alpha(std::size_t size) const
{
    return log_base_value(1.0 / m_alpha, size);
}

double ScapegoatTree::log_base_value(double base, double value)
{
    return std::log(value) / std::log(base);
}

ScapegoatTree::Node * ScapegoatTree::search(Node * v, int value)
{
    if (v == nullptr || v->key == value) {
        return v;
    }
    if (value < v->key) {
        return search(v->left, value);
    }
    return search(v->right, value);
}

void ScapegoatTree::values_impl(Node * v, std::vector<int> & numbers)
{
    if (v == nullptr) {
        return;
    }
    values_impl(v->left, numbers);
    numbers.emplace_back(v->key);
    values_impl(v->right, numbers);
}

ScapegoatTree::Node * ScapegoatTree::find_scapegoat(Node * v, unsigned height) const
{
    if (v == nullptr || height > h_alpha(v->subtree_size)) {
        return v;
    }
    return find_scapegoat(v->parent, height + 1);
}

void ScapegoatTree::rebuild_tree(Node * scapegoat)
{
    if (scapegoat == nullptr) {
        return;
    }
    if (scapegoat == m_root) {
        m_max_tree_size = m_tree_size;
    }
    Node temp;
    Node * parent = scapegoat->parent;
    build_balanced_tree(flatten_tree(scapegoat, &temp), scapegoat->subtree_size);
    if (parent == nullptr) {
        m_root = temp.left;
        temp.left->parent = nullptr;
    }
    else {
        if (parent->left != nullptr && parent->left->key == scapegoat->key) {
            parent->left = temp.left;
        }
        else {
            parent->right = temp.left;
        }
        temp.left->parent = parent;
    }
    temp.left = nullptr;
}

ScapegoatTree::Node * ScapegoatTree::build_balanced_tree(Node * v, std::size_t size)
{
    if (size == 0) {
        v->left = nullptr;
        v->subtree_size = 1;
        return v;
    }
    double variable = (size - 1) / 2.0;
    Node * new_root = build_balanced_tree(v, std::ceil(variable));
    Node * new_last = build_balanced_tree(new_root->right, std::floor(variable));
    new_root->right = new_last->left;
    if (new_last->left != nullptr) {
        new_last->subtree_size -= new_last->left->subtree_size;
        new_last->left->parent = new_root;
        new_root->subtree_size += new_last->left->subtree_size;
    }
    new_last->left = new_root;
    new_root->parent = new_last;
    new_last->subtree_size += new_root->subtree_size;
    return new_last;
}

ScapegoatTree::Node * ScapegoatTree::insert_impl(Node *& v, Node * parent, int value, unsigned & depth)
{
    if (v == nullptr) {
        v = new Node(value);
        v->parent = parent;
        return v;
    }
    ++v->subtree_size;
    ++depth;
    if (value < v->key) {
        return insert_impl(v->left, v, value, depth);
    }
    return insert_impl(v->right, v, value, depth);
}

ScapegoatTree::Node * ScapegoatTree::remove_impl(Node * v, int value)
{
    if (v->parent != nullptr) {
        --v->parent->subtree_size;
    }
    if (value < v->key) {
        v->left = remove_impl(v->left, value);
        if (v->left != nullptr) {
            v->left->parent = v;
        }
    }
    else if (value > v->key) {
        v->right = remove_impl(v->right, value);
        if (v->right != nullptr) {
            v->right->parent = v;
        }
    }
    else {
        if (v->left == nullptr && v->right == nullptr) {
            delete v;
            return nullptr;
        }
        else if (v->left == nullptr || v->right == nullptr) {
            Node * temp = v->left == nullptr ? v->right : v->left;
            v->left = nullptr;
            v->right = nullptr;
            temp->parent = nullptr;
            delete v;
            return temp;
        }
        Node * temp = v->right;
        while (temp->left != nullptr) {
            temp = temp->left;
        }
        v->key = temp->key;
        v->right = remove_impl(v->right, temp->key);
    }
    return v;
}

ScapegoatTree::Node * ScapegoatTree::flatten_tree(Node * v, Node * start)
{
    if (v == nullptr) {
        return start;
    }
    v->right = flatten_tree(v->right, start);
    return flatten_tree(v->left, v);
}

ScapegoatTree::Node::Node(int key)
    : key(key)
{
}

ScapegoatTree::Node::~Node()
{
    delete left;
    delete right;
}

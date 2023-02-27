#include <cstdlib>
#include <cassert>

template <typename Key>
class RBTree {
private:
  struct Node;
  static const Node *nil = Node(0, nullptr, nullptr, nullptr, Node::Color::Black);

public:
  explicit RBTree() {};
  RBTree(const RBTree&) = delete;
  RBTree& operator=(const RBTree&) = delete;

  void Insert(const Key& key);
  void Delete(const Key& key);
  bool Contains(const Key& key);
   
private:
  Node* root;

  Node* FindEqOrLast(const Key& key);
  void LeftRotate(Node *x);
  void RightRotate(Node *y);
  void InsertFixup(Node *z);
  void DeleteFixup(Node *z);
};


template <typename Key>
struct RBTree<Key>::Node {
public:
  enum { Block = 1, Red = 2} Color;

  Node(Key& k, Node* l, Node* r, Node* p, Color c): 
    key(k), left(l), right(r), parent(p), color(c) {}
  
  Color color;
  Key const key;
  Node *left, *right, *parent; 
};

// x.key < y.key
template <typename Key>
void RBTree<Key>::Insert(const Key& key) {
  Node* y = FindEqOrLast(key);

  if (y != nil && y->key == key) {
    return;
  }

  Node* z = new Node(key, nil, nil, y, Node::Color::Red);
  if (y == nil) {
    root = z;
  } else if (z->key < y->key) {
    y->left = z;
  } else {
    y->right = z;
  }

  // z = { left: nil, right: nil, parent: y }
  InsertFixup(z);
}


template <typename Key>
void RBTree<Key>::InsertFixup(Node *z) {
  while (z->parent->color == Node::Red) {
    Node* y;
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right;
      if (y->color == Node::Red) {
        y->color = Node::Black;
        z->parent->color = Node::Black;
        z->parent->parent = Node::Red; 
        z = z->parent->parent;
      } else if (z == z->parent->right) {
        LeftRotate(z);
      }
      z->parent->color = Node::Black;
      z->parent->parent->color = Node::Red;
      RightRotate(z->parent->parent);
    } else {
      y = z->parent->parent->left;
      if (y->color == Node::Red) {
        y->color = Node::Black;
        y->parent->color = Node::Black;
        y->parent->parent->color = Node::Black;
        z = z->parent->parent;
      } else if (z == z->parent->left) {
        RightRotate(z);
      }
      z->parent->color = Node::Black;
      z->parent->parent->color = Node::Red;
    }
  }
  root->color = Node::Black;
}


template <typename Key>
void RBTree<Key>::LeftRotate(Node *x) {
  Node *y = x->left;
  assert(y);

  x->right = y->left;
  if (y->left != nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;
  if (x == root) {
    root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}


template <typename Key>
void RBTree<Key>::RightRotate(Node *y) {
  Node* x = y->left;
  assert(x);

  y->left = x->right; 
  if (x->right != nil) {
    x->right->parent = y;
  }

  x->parent = y->parent;
  if (y == root) {
    root = y; 
  } else if (y == y->parent->left) {
    y->parent->left = x; 
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}


template <typename Key>
typename RBTree<Key>::Node* 
RBTree<Key>::FindEqOrLast(const Key& key) {
  Node* x = root;
  Node* p = nil;  
  while (x != nil) {
    if (x->key < key) {
      x = x->left;
    } else if (x->key > key) {
      x = x->right; 
    } else {
      return x;
    }
  }

  return p;
}

template <typename Key>
bool RBTree<Key>::Contains(const Key& key) {
  Node *x = FindEqOrLast(key);
  return x != nil && x->key == key;
}


// TODO
template <typename Key>
void RBTree<Key>::Delete(const Key& key)
{
  
}

// TODO
template <typename Key>
void RBTree<Key>::DeleteFixup(Node *z)
{

}

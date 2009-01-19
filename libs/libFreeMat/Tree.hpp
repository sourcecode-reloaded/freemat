#ifndef __Tree_hpp__
#define __Tree_hpp__

#include <QString>
#include "Token.hpp"
#include "Array.hpp"
#include "DebugStream.hpp"

// A rewrite of the tree class (yet again).  The lesson learned this time is that we want the
// tree class to be as fast as possible.  And that we want to be able to tag the tree with profile
// information.
class Tree;
class JITFunc;

typedef QList<Tree> TreeList;

typedef enum {
  UNTRIED,
  FAILED,
  SUCCEEDED
} JITState_t;

class TreeNode : public QSharedData {
public:
  Token m_node;
  TreeList m_children;
  JITState_t m_jitstate;
  JITFunc *m_jitfunc;
};

class Tree {
private:
  QSharedDataPointer<TreeNode> d;
public:
  Tree();
  Tree(const Token& tok);
  Tree(TokenValueType token, unsigned position);
  Tree(const Token& tok, Tree child1, Tree child2);
  Tree(const Token& tok, Tree child1);
  Tree(Serialize *s);
  ~Tree();
  inline const Token& node() const {return d->m_node;}
  void print() const;
  inline JITState_t JITState() const {return d->m_jitstate;}
  inline void setJITState(JITState_t t) {d->m_jitstate = t;}
  inline JITFunc* JITFunction() const {return d->m_jitfunc;}
  inline void setJITFunction(JITFunc *t) {d->m_jitfunc = t;}
  inline void rename(TokenValueType newtok) {d->m_node.setValue(newtok);}
  inline unsigned context() const {return d->m_node.position();}
  inline bool valid() const {return !(d->m_node.is(TOK_INVALID));}
  inline Tree first() const {return d->m_children.front();}
  inline Tree second() const {return d->m_children.at(1);}
  inline bool is(TokenValueType tok) const {return (token()==tok);}
  inline TokenValueType token() const {return d->m_node.value();}
  inline int numChildren() const {return d->m_children.size();}
  inline bool hasChildren() const {return (d->m_children.size()>0);}
  inline QString text() const {return d->m_node.text();}
  inline void setText(QString t) {d->m_node.setText(t);}
  inline Array array() const {return d->m_node.array();}
  inline const TreeList& children() const {return d->m_children;}
  inline Tree last() const {return d->m_children.back();}
  inline Tree child(unsigned n) const {return d->m_children.at(n);}
  inline Token& node() {return d->m_node;}
  inline void addChild(Tree t_child) {d->m_children.push_back(t_child);}
  inline void addChildren(Tree child1, Tree child2) {
    d->m_children.push_back(child1);
    d->m_children.push_back(child2);
  }
  void freeze(Serialize *s) const;
};

DebugStream& operator<<(DebugStream& o, const Tree& b);

#endif

/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __Tree_hpp__
#define __Tree_hpp__

#include "FMLib.hpp"
#include "Token.hpp"

namespace FM
{

  class Tree;

  typedef FMVector<Tree> TreeList;

  class TreeNode {
  public:
    Token m_node;
    TreeList m_children;
  };

  class Tree {
  private:
    boost::shared_ptr<TreeNode> d;
  public:
    Tree();
    Tree(const Token& tok);
    Tree(TokenValueType token, unsigned position);
    Tree(const Token& tok, Tree child1, Tree child2);
    Tree(const Token& tok, Tree child1);
    ~Tree();
    inline const Token& node() const {return d->m_node;}
    void print() const;
    FMString str() const;
    inline void rename(TokenValueType newtok) {d->m_node.setValue(newtok);}
    inline unsigned context() const {return d->m_node.position();}
    inline bool valid() const {return !(d->m_node.is(TOK_INVALID));}
    inline Tree first() const {return d->m_children.front();}
    inline Tree second() const {return d->m_children.at(1);}
    inline Tree third() const {return d->m_children.at(2);}
    inline bool is(TokenValueType tok) const {return (token()==tok);}
    inline bool anyChildrenAre(TokenValueType tok) const {
      for (int i=0;i<d->m_children.size();i++) 
	if (d->m_children.at(i).is(tok)) 
	  return true; 
      return false;
    }
    inline TokenValueType token() const {return d->m_node.value();}
    inline int numChildren() const {return d->m_children.size();}
    inline bool hasChildren() const {return (d->m_children.size()>0);}
    inline FMString text() const {return d->m_node.text();}
    inline void setText(FMString t) {d->m_node.setText(t);}
    inline const TreeList& children() const {return d->m_children;}
    inline const Tree last() const {return d->m_children.back();}
    inline Tree child(unsigned n) const {return d->m_children.at(n);}
    inline Token& node() {return d->m_node;}
    inline void addChild(Tree t_child) {d->m_children.push_back(t_child);}
    inline void addChildren(Tree child1, Tree child2) {
      d->m_children.push_back(child1);
      d->m_children.push_back(child2);
    }
  };
}

#endif

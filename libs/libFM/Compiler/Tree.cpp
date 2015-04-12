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
#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"

using namespace FM;

Tree::Tree() {
  d = boost::shared_ptr<TreeNode>(new TreeNode);
  d->m_node = Token(TOK_INVALID);
}

Tree::Tree(const Token& tok) {
  d = boost::shared_ptr<TreeNode>(new TreeNode);
  d->m_node = tok;
}

Tree::Tree(TokenValueType token, unsigned position) {
  d = boost::shared_ptr<TreeNode>(new TreeNode);
  d->m_node = Token(token,position);
}

Tree::Tree(const Token& tok, Tree child1, Tree child2) {
  d = boost::shared_ptr<TreeNode>(new TreeNode);
  d->m_node = tok;
  d->m_children.push_back(child1);
  d->m_children.push_back(child2);
}

Tree::Tree(const Token& tok, Tree child1) {
  d = boost::shared_ptr<TreeNode>(new TreeNode);
  d->m_node = tok;
  d->m_children.push_back(child1);
}

Tree::~Tree() {}

void Tree::print() const {
  std::cout << this->str();
}

static int indentlevel = 0;
FMString Tree::str() const {
  FMString ret;
  ret = FMString(indentlevel,FMChar(' '));
  ret += TokenToString(d->m_node) + "  <" + Stringify(LineNumber(d->m_node.position())) + ">\n";
  indentlevel += 3;
  for (unsigned i=0;i<d->m_children.size();i++)
    ret += d->m_children[i].str();
  indentlevel -= 3;
  return ret;
}

static int indentlvl = 0;
std::ostream& operator<<(std::ostream& o, const Tree& b) {
    for( int i=0; i<indentlvl; i++ )
	o << "  ";
    o << b.node();
    indentlvl += 1;
    for (int i=0;i< b.numChildren();i++){
	o << b.child(i);
    }
    indentlvl -= 1;
    return o;
}

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
#ifndef __JITAnalysis_hpp__
#define __JITAnalysis_hpp__
#include "JIT.hpp"
#include "Tree.hpp"
#include "SymbolTable.hpp"
#include "Array.hpp"

class VariableInfo {
	bool isUndefined; 
	bool isScalar;
	int argument_num;
	DataClass type;

	friend class JITVariableTable;

public:
	VariableInfo(bool is_Undefined, bool is_scalar, int arg_num, DataClass base_type) :
		isUndefined(is_Undefined), isScalar(is_scalar), argument_num(arg_num), type(base_type) {}
	VariableInfo(const VariableInfo &other) { this->isUndefined = other.isUndefined; this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->type = other.type; }
	VariableInfo &operator=(const VariableInfo &other) { this->isUndefined = other.isUndefined; this->isScalar = other.isScalar; this->argument_num = other.argument_num; this->type = other.type; return *this;}
	VariableInfo() {} //need default constructor for SymbolTable
	bool isScalarVariable(void) const { return isScalar; }
	void setScalar(bool Scalar) { isScalar = Scalar; }
	DataClass data_class( void ) const { return type; }
};

class JITVariableTable{
private:
	Interpreter *eval;
	FM::SymbolTable<VariableInfo> symbols;
public:
	JITVariableTable(Interpreter *p_eval) : eval(p_eval){};
	void rhs_assignment( QString name, const VariableInfo& lhs_type );
	void rhs_index_assignment( QString name, const VariableInfo& lhs_type );
	void new_loop_index( QString name, const VariableInfo& lhs_type );
	
private:
	void add_variable( QString name, DataClass type, bool isScalar, bool isLoopVariable = false );
};

class JITAnalysis{
private:
	JIT *jit;
	Interpreter *eval;
	JITVariableTable variables;

public:
	JITAnalysis(Interpreter *p_eval) : variables( p_eval), eval(p_eval) { jit = JIT::Instance(); }
	void ProcessTree( const Tree& t );

private:
	void process_statement_type( const Tree& t );
	void process_assignment( const Tree& t );
	void process_for_block( const Tree& t );
	void process_if_statement( const Tree& t );
	VariableInfo process_expression( const Tree& t );
	void process_statement(const Tree & t);
	void process_block(const Tree & t);
	VariableInfo process_binary_operation( const Tree& t );
	VariableInfo process_variable( const Tree& t );
	DataClass BinaryOpResultType( DataClass v1, DataClass v2 );

};

#endif
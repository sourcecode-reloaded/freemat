/*
* Copyright (c) 2009 Samit Basu, Eugene Ingerman
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
// Just for fun, mind you....
//

#ifdef HAVE_LLVM

#include "DebugStream.hpp"
#include "JITAnalysis.hpp"
#include "Context.hpp"
#include "Interpreter.hpp"

void JITAnalysis::add_variable( QString name, DataClass type, bool isScalar, bool isLoopVariable )
{
	ArrayReference ptr(eval->getContext()->lookupVariable(name));
	if (!ptr.valid()){
		symbols.insertSymbol( name, VariableInfo( false, isScalar, 0, type ) );
		dbout << "Defining vars" << '\n';
	}
	else{
		DataClass aclass = ptr->dataClass();
		dbout << "Defining vars" << '\n';
	}

}

void JITAnalysis::ProcessTree( const Tree &t)
{
	dbout << t << '\n';
	process_statement_type( t );
}

void JITAnalysis::process_statement_type(const Tree &t)
{
	switch(t.token()) {
		  case '=': 
			  process_assignment(t);
			  break;
		  case TOK_FOR:
			  process_for_block(t);
			  break;
		  case TOK_IF:
			  process_if_statement(t);
			  break;
		  //case TOK_MULTI:   	  throw Exception("multi function calls do not JIT compile");
		  //case TOK_SPECIAL:		  throw Exception("special function calls do not JIT compile");
		  //case TOK_WHILE:		  throw Exception("nested while loops do not JIT compile");
		  //case TOK_BREAK:       throw Exception("break is not currently handled by the JIT compiler");
		  //case TOK_CONTINUE:    throw Exception("continue is not currently handled by the JIT compiler");
		  //case TOK_DBSTEP:      throw Exception("dbstep is not currently handled by the JIT compiler");
		  //case TOK_DBTRACE:     throw Exception("dbtrace is not currently handled by the JIT compiler");
		  //case TOK_RETURN:      throw Exception("return is not currently handled by the JIT compiler");
		  //case TOK_SWITCH:      throw Exception("switch is not currently handled by the JIT compiler");
		  //case TOK_TRY:         throw Exception("try is not currently handled by the JIT compiler");
		  //case TOK_QUIT:        throw Exception("quit is not currently handled by the JIT compiler");
		  //case TOK_RETALL:      throw Exception("retall is not currently handled by the JIT compiler");
		  //case TOK_KEYBOARD:    throw Exception("keyboard is not currently handled by the JIT compiler");
		  //case TOK_GLOBAL:      throw Exception("global is not currently handled by the JIT compiler");
		  //case TOK_PERSISTENT:  throw Exception("persistent is not currently handled by the JIT compiler");
		  case TOK_EXPR:
			  process_expression(t.first());
			  break;
		  case TOK_NEST_FUNC:
			  break;
		  default:
			  //throw Exception("Unrecognized statement type");
			  ;
	}
}

void JITAnalysis::process_assignment( const Tree& t )
{
	dbout << "Var: " << t.first().text() << " #children: " << t.numChildren() << '\n';
}

void JITAnalysis::process_for_block( const Tree& t )
{
	dbout << "For Parent: " << t.text() << " #children: " << t.numChildren() << '\n';
	dbout << "For Block. Var: " << t.first().text() << " #children: " << t.first().numChildren() << '\n';
	dbout << "For Block child 1. Var: " << t.first().first().text() << " #children: " << t.first().first().numChildren() << '\n';


	QString loop_variable_name( t.first().first().text() );
	
	add_variable( loop_variable_name, Double, true, true );

	
	dbout << "For Block child 2. Var: " << t.first().second().text() << " #children: " << t.first().second().numChildren() << '\n';
	process_statement_type( t.second() );
}

void JITAnalysis::process_if_statement( const Tree& t )
{
}

void JITAnalysis::process_expression( const Tree& t )
{
}


#endif
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

void JITVariableTable::add_variable( QString name, DataClass type, bool isScalar, bool isLoopVariable )
{
	if (!symbols.hasSymbol( name )){
		symbols.insertSymbol( name, VariableInfo( false, isScalar, 0, type ) );
		dbout << "Defining vars new: " << name << '\n';
	}
	else{
		VariableInfo* vi = symbols.findSymbol( name );
		
		if( type != vi->data_class() ){
			symbols.deleteSymbol( name );
			symbols.insertSymbol( name, VariableInfo( true, false, 0, Invalid ) );
			dbout << "Defining vars existing " << name << '\n';
		}
		
	}

}

void JITVariableTable::rhs_index_assignment( QString name, const VariableInfo& lhs_type )
{
	ArrayReference ptr(eval->getContext()->lookupVariable(name));
	if (!ptr.valid()){
		add_variable( name, lhs_type.data_class(), lhs_type.isScalarVariable() );
	}
	else{

		add_variable( name, ptr->dataClass(), ptr->isScalar() );
		//assert(0); //TODO: check type
	}
}

void JITVariableTable::rhs_assignment( QString name, const VariableInfo& lhs_type )
{
	add_variable( name, lhs_type.data_class(), lhs_type.isScalarVariable(), false );
}

void JITVariableTable::new_loop_index( QString name, const VariableInfo& lhs_type )
{
	add_variable( name, lhs_type.data_class(), lhs_type.isScalarVariable(), true );
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

void JITAnalysis::process_statement(const Tree & t) {
	if (t.is(TOK_STATEMENT) && 
		(t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
		t.first().is(TOK_MULTI) || t.first().is('=')))
		throw Exception("JIT compiler doesn't work with verbose statements");
	process_statement_type(t.first());
}


void JITAnalysis::process_block(const Tree & t) {
	const TreeList &statements(t.children());
	for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++) 
		process_statement(*i);
}


void JITAnalysis::process_assignment( const Tree& t )
{
	const Tree & s(t.first());
	QString symbol_prefix("");
	QString symname(symbol_prefix+s.first().text());

	const Tree & rhs( t.second() );
	VariableInfo rhs_type;

	if( s.numChildren() == 1 ){
		dbout << "Var: " << symname << " variable assignment on lhs\n";
		rhs_type = process_expression( rhs );
		variables.rhs_assignment( symname, rhs_type );
	}
	else{
		dbout << "Var: " << symname << " indexed assignment on lhs\n";
		dbout << "Num children " << s.numChildren() << "\n";
		rhs_type = process_expression( rhs );
		variables.rhs_index_assignment( symname, rhs_type );
	}

	//JITScalar rhs(compile_expression(t.second()));
	//SymbolInfo *v = symbols.findSymbol(symname);
	//if (!v) {
	//	if (s.numChildren() == 1)
	//		v = add_argument_scalar(symname,rhs,false);
	//	else
	//		v = add_argument_array(symname, true /*createIfMissing*/ );
	//	if (!v) throw Exception("Undefined variable reference:" + symname);
	//}
	//if (s.numChildren() == 1) {
	//	if (v->type != jit->TypeOf(rhs))
	//		throw Exception("polymorphic assignment to scalar detected.");
	//	if (!v->isScalar)
	//		throw Exception("scalar assignment to array variable.");
	//	jit->Store(rhs, v->address);
	//	return;
	//}
	//if (s.numChildren() > 2)
	//	throw Exception("multiple levels of dereference not handled yet...");
	//if (v->isScalar)
	//	throw Exception("array indexing of scalar values...");
	//const Tree & q(s.second());
	//if (!q.is(TOK_PARENS))
	//	throw Exception("non parenthetical dereferences not handled yet...");
	//if (q.numChildren() == 0)
	//	throw Exception("Expecting at least 1 array reference for dereference...");
	//if (q.numChildren() > 2)
	//	throw Exception("Expecting at most 2 array references for dereference...");
	//if (q.numChildren() == 1) {
	//	JITScalar arg1 = jit->ToDouble(compile_expression(q.first()));
	//	JITScalar success_code;
	//	if (jit->IsDouble(v->type))
	//		success_code = jit->Call(func_vector_store_double, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
	//		jit->ToType(rhs,v->type));
	//	else if (jit->IsFloat(v->type))
	//		success_code = jit->Call(func_vector_store_float, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
	//		jit->ToType(rhs,v->type));
	//	else if (jit->IsBool(v->type))
	//		success_code = jit->Call(func_vector_store_bool, this_ptr, jit->DoubleValue(v->argument_num), arg1, 
	//		jit->ToType(rhs,v->type));
	//	else
	//		throw Exception("unhandled type for vector store");
	//	handle_success_code(success_code);
	//	return;
	//} else if (q.numChildren() == 2) {
	//	JITScalar arg1 = jit->ToDouble(compile_expression(q.first()));
	//	JITScalar arg2 = jit->ToDouble(compile_expression(q.second()));
	//	JITScalar success_code;
	//	if (jit->IsDouble(v->type))
	//		success_code = jit->Call(func_matrix_store_double, this_ptr, jit->DoubleValue(v->argument_num), 
	//		arg1, arg2, jit->ToType(rhs,v->type));
	//	else if (jit->IsFloat(v->type))
	//		success_code = jit->Call(func_matrix_store_float, this_ptr, jit->DoubleValue(v->argument_num), 
	//		arg1, arg2, jit->ToType(rhs,v->type));
	//	else if (jit->IsBool(v->type))
	//		success_code = jit->Call(func_matrix_store_bool, this_ptr, jit->DoubleValue(v->argument_num), 
	//		arg1, arg2, jit->ToType(rhs,v->type));
	//	else
	//		throw Exception("unhandled type for matrix store");
	//	handle_success_code(success_code);
	//	return;    
	//}


	dbout << "Var: " << t.first().text() << " #children: " << t.numChildren() << '\n';
}

void JITAnalysis::process_for_block( const Tree& t )
{
	dbout << "For Parent: " << t.text() << " #children: " << t.numChildren() << '\n';
	dbout << "For Block. Var: " << t.first().text() << " #children: " << t.first().numChildren() << '\n';
	dbout << "For Block child 1. Var: " << t.first().first().text() << " #children: " << t.first().first().numChildren() << '\n';


	QString loop_variable_name( t.first().first().text() );
	
	variables.new_loop_index( loop_variable_name, VariableInfo( false, true, 0, Double ) );

	
	dbout << "For Block child 2. Var: " << t.first().second().text() << " #children: " << t.first().second().numChildren() << '\n';
	process_block( t.second() );
}

void JITAnalysis::process_if_statement( const Tree& t )
{
}

VariableInfo JITAnalysis::process_variable( const Tree& t )
{
	dbout << "---->  process_variable: " << t.first().text() << " " << t.numChildren() << "\n";
	VariableInfo vi( false, false, 0, Double );
	return vi;
}

DataClass JITAnalysis::BinaryOpResultType( DataClass v1, DataClass v2 )
{
	bool v1FP = false; 
	bool v2FP = false;
	DataClass retType = Invalid;

	//if one of the arguments is unknown - the result has to be unknown
	if( v1==Invalid || v2==Invalid )
		return Invalid;

	v1FP = ( v1 == Double || v1 == Float )? true:false;
	v2FP = ( v2 == Double || v2 == Float )? true:false;
	//float+double => float
	if( v1FP && v2FP ){
		retType = ( (v1==Double) && (v2==Double) )? Double:Float;
		return retType;
	}

	//integer type can only be combined with the same integer type
	if( v1 == v2 )
		return v1;
	
	throw Exception("Integers can only be combined with the same type integers or with doubles");
	return Invalid;
}

VariableInfo JITAnalysis::process_binary_operation( const Tree& t )
{
	VariableInfo v1 = process_expression( t.first() );
	VariableInfo v2 = process_expression( t.second() );

	bool isResultScalar = v1.isScalarVariable() && v2.isScalarVariable();
	
	DataClass retType = BinaryOpResultType( v1.data_class(), v2.data_class() );

	dbout << "---->  process_variable: " << t.first().text() << " " << t.numChildren() << "\n";
	VariableInfo vi( false, false, 0, retType );
	return vi;
}


VariableInfo JITAnalysis::process_expression( const Tree& t )
{
	switch(t.token()) {
		  case TOK_VARIABLE:     return process_variable(t);
/*		  case TOK_REAL:
		  case TOK_REALF:
			  if( t.array().isScalar() ){
				  switch( t.array().dataClass() ){
					  case Bool:
						  return jit->BoolValue( t.array().constRealScalar<bool>() );
					  case Float:
						  return jit->FloatValue( t.array().constRealScalar<float>() );
					  case Double:
						  return jit->DoubleValue( t.array().constRealScalar<double>() );
					  default:
						  throw Exception("Unsupported scalar type.");
				  }
			  }
			  else
				  throw Exception("Unsupported type.");
		  case TOK_STRING:
		  case TOK_END:
		  case ':':
		  case TOK_MATDEF: 
		  case TOK_CELLDEF:      throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
*/
		  case '+':
		  case '-':
		  case '*': 
		  case TOK_DOTTIMES: 
		  case '/': 
		  case TOK_DOTRDIV:
			  return process_binary_operation( t );
/*
		  case '\\': 
		  case TOK_DOTLDIV: 
			  return jit->Div(compile_expression(t.second()),compile_expression(t.first()));
		  case TOK_SOR: 
			  return compile_or_statement(t);
		  case '|':
			  return jit->Or(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_SAND: 
			  return compile_and_statement(t);
		  case '&': 
			  return jit->And(compile_expression(t.first()),compile_expression(t.second()));
		  case '<': 
			  return jit->LessThan(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_LE: 
			  return jit->LessEquals(compile_expression(t.first()),compile_expression(t.second()));
		  case '>': 
			  return jit->GreaterThan(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_GE: 
			  return jit->GreaterEquals(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_EQ: 
			  return jit->Equals(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_NE: 
			  return jit->NotEqual(compile_expression(t.first()),compile_expression(t.second()));
		  case TOK_UNARY_MINUS: 
			  return jit->Negate(compile_expression(t.first()));
		  case TOK_UNARY_PLUS: 
			  return compile_expression(t.first());
		  case '~': 
			  return jit->Not(compile_expression(t.first()));
		  case '^':               throw Exception("^ is not currently handled by the JIT compiler");
		  case TOK_DOTPOWER:      throw Exception(".^ is not currently handled by the JIT compiler");
		  case '\'':              throw Exception("' is not currently handled by the JIT compiler");
		  case TOK_DOTTRANSPOSE:  throw Exception(".' is not currently handled by the JIT compiler");
		  case '@':               throw Exception("@ is not currently handled by the JIT compiler");
		  default:                throw Exception("Unrecognized expression!");
	*/}  
}


#endif
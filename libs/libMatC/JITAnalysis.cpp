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

extern QString uid_string(int uid);

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

VariableInfo* JITVariableTable::find_by_name( const QString name )
{ 
	if (!symbols.hasSymbol( name )){ //we first check our table, if missing - we ask the interpreter
		ArrayReference ptr(eval->getContext()->lookupVariable(name));
		if (ptr.valid()){
			add_variable( name, ptr->dataClass(), ptr->isScalar() );
		}
		//if symbol is unknown to the interpreter findSymbol at the end will fail, as it should
	}
	return symbols.findSymbol( name ); 
}

void JITVariableTable::rhs_index_assignment( QString name, const VariableInfo& lhs_type )
{
	VariableInfo* vi = find_by_name( name );
	if( !vi ){
		add_variable( name, lhs_type.data_class(), false /*lhs_type.isScalarVariable()*/ );
	}
	else{
		if( vi->isScalarVariable() ){ //index assignment to vector variable - assume it is vector type
			vi->setScalar( false /*lhs_type.isScalarVariable()*/ );
		}
		//since vector assignment can't change variable type - don't match it with lhs.
	}
}

void JITVariableTable::function_arg( QString name, const VariableInfo& vi )
{
	add_variable( name, vi.data_class(), vi.isScalarVariable(), false );
}

void JITVariableTable::internal_variable( QString name, const VariableInfo& vi )
{
	add_variable( name, vi.data_class(), vi.isScalarVariable(), false );
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


MultiVariableInfo JITAnalysis::process_function( const Tree& t )
{
	MultiVariableInfo ret;
	// First, make sure it is a function
	QString symname(t.first().text());
	FuncPtr funcval;
	if (!eval->lookupFunction(symname,funcval)) 
		throw Exception("Couldn't find function " + symname);
	if (funcval->type() != FM_M_FUNCTION)
		throw Exception("Expected M function");
	MFunctionDef *fptr = (MFunctionDef*) funcval;
	if ((fptr->inputArgCount() < 0) || (fptr->outputArgCount() < 0))
		throw Exception("Variable argument functions not handled");
	if (fptr->nestedFunction /*ei || fptr->capturedFunction*/)
		throw Exception("Nested and/or captured functions not handled");
	if (fptr->scriptFlag) 
		throw Exception("scripts not handled");

	// Set up the prefix
	QString new_symbol_prefix = symbol_prefix + "$" + symname + "_" + uid_string(uid);
	uid++;
	// Loop through the arguments to the function,
	// and map them from the defined arguments of the tree
	if (t.numChildren() < 2) 
		throw Exception("function takes no arguments - not currently supported");
	const Tree & s(t.second());
	int args_defed = fptr->arguments.size();
	if (args_defed > s.numChildren())
		args_defed = s.numChildren();
	for (int i=0;i<args_defed;i++) {
		VariableInfo vi = process_expression(s.child(i));
		variables.function_arg( new_symbol_prefix + fptr->arguments[i], vi );
	}
	variables.internal_variable( new_symbol_prefix+"nargout", VariableInfo( false, true, 0, Double ));
	variables.internal_variable( new_symbol_prefix+"nargin", VariableInfo( false, true, 0, Double ));

	QString save_prefix = symbol_prefix;
	symbol_prefix = new_symbol_prefix;
	process_block(fptr->code);

	//does not yet handle correctly variadic functions
	for( int k = 0; k < fptr->outputArgCount(); ++k ){
		VariableInfo *v = variables.find_by_name(new_symbol_prefix+fptr->returnVals[k]);
		if (!v) throw Exception("function failed to define return value");
		ret.append( *v );
	}
	symbol_prefix = save_prefix;
	return ret;
}

VariableInfo JITAnalysis::process_variable( const Tree& t )
{
	QString symname = symbol_prefix+t.first().text();

	dbout << "---->  process_variable: " << symname << " " << t.numChildren() << "\n";
	VariableInfo* vi = variables.find_by_name( symname );	
	if ( !vi ) {
		FuncPtr funcval;
		if (!eval->lookupFunction(symname,funcval)) 
			throw Exception("Couldn't find function " + symname);
		funcval->updateCode(eval);
		MultiVariableInfo mvi = process_function( t );
		if( !mvi.empty() )
			return( mvi[0] ); 
	}

	if( !vi )
		throw Exception("Use of uninitialized variable on the rhs");
	return *vi;
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
	VariableInfo vi( false, isResultScalar, 0, retType );
	return vi;
}


VariableInfo JITAnalysis::process_expression( const Tree& t )
{
	switch(t.token()) {
		  case TOK_VARIABLE:     return process_variable(t);

		  case TOK_REAL:
		  case TOK_REALF:
			  {
				  VariableInfo vi;
				  if( t.array().isScalar() ){
					  vi.setScalar( true );
				  }
				  vi.setDataClass( t.array().dataClass() );
				  vi.setDefined();
				  return vi;
			  }
			  /*		  case TOK_STRING:
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
			  */
	} 
	throw Exception(" process_expression missing case ");
}


#endif
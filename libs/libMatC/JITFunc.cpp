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
// New plans for the JIT:
//
// Type complexity has been greatly reduced in FreeMat4.  Consider the classic example
// of the Toeplitz matrix creation loop in FreeMat4:
//
//  A = zeros(512);
//  for i=1:512;
//    for j=1:512;
//      A(j,i) = j-i+1;
//    end
//  end
//
// In FreeMat3, this loop would have required the handling of at least 3 seperate types
// (int32, float, double), and would not even have JIT compiled due to the polymorphic
// assignments to the A matrix.
//
// In FreeMat4, the loop is much simpler.  Everything is a double type.  If we start
// with a JIT that only processes double types, I think we will cover a significant
// portion of MATLAB-style code.  We also avoid the issues around saturation arithmetic
// that plague the handling of integer types.
//
// Do we handle float types too?
//
// Consider the problem of extending the JIT to handle general expressions.  For example:
//   for i=1:512
//     for j=1:512
//       b = [i;j];
//       c = T*b;
//       A(i,j) = A(i,j) + proj(c);
//     end
//   end
// In this case, we have to handle a bunch of operations.
//
// Generally speaking, we now have the situation in which an expression, when evaluated can be an
// array as well as (or instead of) a scalar.  However, the actual array is not available when
// the JIT is compiled.  Only the type and the fact that it is an array is available.  What is also
// apparent is that the array has to be stored somewhere.  The JIT will then have to create a symbol
// table.  And what happens to intermediate values then?  In the limit, the JIT must address all of
// the same conditions as the interpreter.
//
// In the example above,
//

#ifdef HAVE_LLVM

#include <QString>
#include "JITFunc.hpp"
#include "Context.hpp"
#include "Interpreter.hpp"
#include "Array.hpp"
#include <sstream>
#include <math.h>

//#if defined(_MSC_VER ) || defined( __MINGW32__ )
#if defined(_WIN32 )
#define JIT_EXPORT __declspec( dllexport )
#else
#define JIT_EXPORT
#endif

JITFunc *save_this;


static JITFunction func_scalar_load_double, func_scalar_load_float, func_scalar_load_bool;
static JITFunction func_scalar_store_double, func_scalar_store_float, func_scalar_store_bool;
static JITFunction func_vector_load_double, func_vector_load_float, func_vector_load_bool;
static JITFunction func_vector_store_double, func_vector_store_float, func_vector_store_bool;
static JITFunction func_matrix_load_double, func_matrix_load_float, func_matrix_load_bool;
static JITFunction func_matrix_store_double, func_matrix_store_float, func_matrix_store_bool;
static JITFunction func_check_for_interrupt;
static JITFunction func_niter_for_loop, func_debug_out_d;

SymbolInfo* JITFunc::add_argument_array(QString name, bool createIfMissing=false) {
    if (symbol_prefix.size() > 0)
        return NULL;
    ArrayReference ptr(eval->getContext()->lookupVariable(name));
    DataClass aclass = Invalid;

    if (!ptr.valid()) {
        if ( createIfMissing ) {
            symbols.insertSymbol(name,SymbolInfo(false,argument_count++, Double));
	    QTextStream(&init_block) << "Array* " << name << " = new Array(Double);\n"; 
            return symbols.findSymbol(name);
        }
        else {
            return NULL;
        }
    }

    aclass = ptr->dataClass();
    // Map the array class to an llvm type
    QTextStream(&init_block) << "Array* " << name << " = new( " << ptr->address() << ") Array(  (DataClass)"<< aclass << ");\n"; 
    symbols.insertSymbol(name,SymbolInfo(false,argument_count++,aclass));
    return symbols.findSymbol(name);
}


// //TODO: fix this
// SymbolInfo* JITFunc::define_local_symbol(QString name, JITScalar val) {
//     if (!val) throw Exception("undefined variable or argument " + name);
//     JITBlock ip(jit->CurrentBlock());
//     jit->SetCurrentBlock(prolog);
//     JITScalar address = jit->Alloc(jit->TypeOf(val),name);
//     symbols.insertSymbol(name,SymbolInfo(true,-1,address,jit->TypeOf(val)));
//     jit->SetCurrentBlock(ip);
//     jit->Store(val,address);
//     return symbols.findSymbol(name);
// }

// FIXME - Simplify
SymbolInfo* JITFunc::add_argument_scalar(QString name, QString val, bool override) {
    DataClass aclass;
/*    if (symbol_prefix.size() > 0)
        return define_local_symbol(name,val);*/
    
    ArrayReference ptr(eval->getContext()->lookupVariable(name));
    //aclass = Invalid;
    //if (!val && !ptr.valid()) return NULL;
    if (!ptr.valid() || override) {
        aclass = Double;
	symbols.insertSymbol(name,SymbolInfo(true,argument_count++,aclass));
	QTextStream(&init_block) << "Array* " << name << " = new Array(  "<< val << ");\n"; 
	return symbols.findSymbol(name);
	
    } else {
        aclass = ptr->dataClass();
    }
    // Map the array class to an llvm type
    //TODO: Fix this
    QTextStream(&init_block) << "Array* " << name << " = new(" << ptr->address() << ") Array(  "<< val << ");\n"; 
    symbols.insertSymbol(name,SymbolInfo(true,argument_count++,aclass));
    return symbols.findSymbol(name);
}

JITFunc::JITFunc(Interpreter *p_eval) {
    jit = JIT::Instance();
    eval = p_eval;


    QTime t;

    jc = new JITCompiler();
    t.start();
    jc->add_bc_file("/home/eugening/freemat/build/libs/libMatC/libF.bc");
    dbout << "BC add time " << t.restart() << " ms\n";

    jc->add_system_include_path("/home/eugening/freemat/FreeMat4/libs/libFreeMat");
    jc->add_system_include_path("/home/eugening/freemat/FreeMat4/libs/libXP");
    jc->add_system_include_path("/home/eugening/freemat/FreeMat4/libs/libMex");
    jc->add_system_include_path("/home/eugening/freemat/FreeMat4/libs/libMatC");
    jc->add_system_include_path("/usr/include/QtCore");
    jc->add_system_include_path("/usr/include/QtGui");
    jc->add_system_include_path("/usr/include/c++/4.5");
    jc->add_system_include_path("/usr/include/c++/4.5/i586-suse-linux");
    jc->add_system_include_path("/usr/local/include");
    jc->add_system_include_path("/usr/local/lib/clang/2.9/include");

    code.append("#include <QtCore/QVector>\n    #include <Array.hpp>\n");
    code.append("extern \"C\" int f_t(){ %1; \n}\n");

    jc->add_source_from_string(code, "a.cpp");
    dbout << "Add source time " << t.restart() << " ms\n";
    jc->compile();
    dbout << "Compile time " << t.restart() << " ms\n";
    jc->run_function("f_t");
    dbout << "Run once time " << t.restart() << " ms\n";
    jc->run_function("f_t");
    dbout << "Run twice time " << t.elapsed() << " ms\n";

}

QString JITFunc::compile_block(const Tree & t) {
    QString c;
    const TreeList &statements(t.children());
    for (TreeList::const_iterator i=statements.begin();i!=statements.end();i++)
        c.append(compile_statement(*i));
    return c;
}

QString JITFunc::compile_statement_type(const Tree& t) {
    QString c;
    switch (t.token()) {
    case '=':
        c.append(compile_assignment(t));
        break;
    case TOK_MULTI:
        throw Exception("multi function calls do not JIT compile");
    case TOK_SPECIAL:
        throw Exception("special function calls do not JIT compile");
    case TOK_FOR:
        c.append(compile_for_block(t));
        break;
    case TOK_WHILE:
        throw Exception("nested while loops do not JIT compile");
    case TOK_IF:
        c.append(compile_if_statement(t));
        break;
    case TOK_BREAK:
        throw Exception("break is not currently handled by the JIT compiler");
    case TOK_CONTINUE:
        throw Exception("continue is not currently handled by the JIT compiler");
    case TOK_DBSTEP:
        throw Exception("dbstep is not currently handled by the JIT compiler");
    case TOK_DBTRACE:
        throw Exception("dbtrace is not currently handled by the JIT compiler");
    case TOK_RETURN:
        throw Exception("return is not currently handled by the JIT compiler");
    case TOK_SWITCH:
        throw Exception("switch is not currently handled by the JIT compiler");
    case TOK_TRY:
        throw Exception("try is not currently handled by the JIT compiler");
    case TOK_QUIT:
        throw Exception("quit is not currently handled by the JIT compiler");
    case TOK_RETALL:
        throw Exception("retall is not currently handled by the JIT compiler");
    case TOK_KEYBOARD:
        throw Exception("keyboard is not currently handled by the JIT compiler");
    case TOK_GLOBAL:
        throw Exception("global is not currently handled by the JIT compiler");
    case TOK_PERSISTENT:
        throw Exception("persistent is not currently handled by the JIT compiler");
    case TOK_EXPR:
        c.append(compile_expression(t.first()));
        break;
    case TOK_NEST_FUNC:
        break;
    default:
        throw Exception("Unrecognized statement type");
    }
    return c;
}

QString JITFunc::compile_statement(const Tree& t) {
    if (t.is(TOK_STATEMENT) &&
            (t.first().is(TOK_EXPR) || t.first().is(TOK_SPECIAL) ||
             t.first().is(TOK_MULTI) || t.first().is('=')))
        throw Exception("JIT compiler doesn't work with verbose statements");
    return compile_statement_type(t.first());
}

QString JITFunc::compile_constant_function(QString symname) {
    //TODO: check that the symbol exists
    return (symname); 
}

QString JITFunc::compile_built_in_function_call(const Tree & t) {
    QString c;
    // First, make sure it is a function
    QString symname(t.first().text());
    FuncPtr funcval;
    if (!eval->lookupFunction(symname,funcval))
        throw Exception(QString("Couldn't find function ") + symname);
    if (t.numChildren() != 2)
        return compile_constant_function(symname);
    // Evaluate the argument
    const Tree & s(t.second());
    if (!s.is(TOK_PARENS))
        throw Exception("Expecting function arguments.");
    if (s.numChildren() > 1)
        throw Exception("Cannot JIT functions that take more than one argument");
    if (s.numChildren() == 0)
        return compile_constant_function(symname);
    else {
        QString arg = compile_expression(s.first());
	QTextStream(&c) << symname << "( " << arg << " )";
    }
    return c;
}

static QString uid_string(int uid) {
    return QString("%1").arg(uid);
}

QString JITFunc::compile_m_function_call(const Tree & t) {
    QString output;
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
        QString arg = compile_expression(s.child(i));
        output.append(define_local_symbol(new_symbol_prefix + fptr->arguments[i],arg));
    }
    output.append(define_local_symbol(new_symbol_prefix+"nargout","1"));
    output.append(define_local_symbol(new_symbol_prefix+"nargin","args_defed"));
    // compile the code for the function
    fptr->code.print();
    QString save_prefix = symbol_prefix;
    symbol_prefix = new_symbol_prefix;
    output.append(compile_block(fptr->code));
    // Lookup the result and return it
    SymbolInfo *v = symbols.findSymbol(new_symbol_prefix+fptr->returnVals[0]);
    if (!v) throw Exception("function failed to define return value");
    //TODO: Not sure how to return params
    //QTextStream(&output) << (new_symbol_prefix+fptr->returnVals[0]) << " = " << 
    symbol_prefix = save_prefix;
    return jit->Load(v->address);
}

QString JITFunc::compile_function_call(const Tree & t) {
    // First, make sure it is a function
    QString symname(t.first().text());
    FuncPtr funcval;
    if (!eval->lookupFunction(symname,funcval))
        throw Exception("Couldn't find function " + symname);
    funcval->updateCode(eval);
    if (funcval->type() == FM_BUILT_IN_FUNCTION)
        return compile_built_in_function_call(t);
    if (funcval->type() == FM_M_FUNCTION)
        return compile_m_function_call(t);
    if (t.numChildren() != 2)
        return compile_constant_function(symname);
    throw Exception("Unsupported function type");
}

// void JITFunc::handle_success_code(JITScalar success_code) {
//     JITBlock if_failed = jit->NewBlock("exported_call_failed");
//     JITBlock if_success = jit->NewBlock("exported_call_sucess");
//     // Add the branch logic
//     jit->Branch(if_success,if_failed,success_code);
//     jit->SetCurrentBlock(if_failed);
//     jit->Store(success_code,retcode);
//     jit->Jump(epilog);
//     jit->SetCurrentBlock(if_success);
// }

QString JITFunc::compile_rhs(const Tree & t) {
    QString output;
    QString symname(symbol_prefix+t.first().text());
    SymbolInfo *v = symbols.findSymbol(symname);
    if (!v) {
        v = add_argument_array(symname);
        if (!v)
            return compile_function_call(t);
    }
    
    if (t.numChildren() > 2)
        throw Exception("multiple levels of dereference not handled yet...");

    const Tree & s(t.second());
    if (!s.is(TOK_PARENS))
        throw Exception("non parenthetical dereferences not handled yet...");
    if (s.numChildren() == 0)
        throw Exception("Expecting at least 1 array reference for dereference...");
    if (s.numChildren() > 2)
        throw Exception("Expecting at most 2 array references for dereference...");
    if (s.numChildren() == 1) {
        QTextStream(&output) << symname << ".get(NTuple( " << (compile_expression(s.first())) << " ) ) ";
	return output;
    } else if (s.numChildren() == 2) {
	QTextStream(&output) << symname << ".get(NTuple( " << (compile_expression(s.first())) << ", " << (compile_expression(s.second())) << " ) ) ";
        return output;
    }
    throw Exception("dereference not handled yet...");
}

QString JITFunc::compile_expression(const Tree & t) {
    QString output;
    switch (t.token()) {
    case TOK_VARIABLE:
        return compile_rhs(t);
    case TOK_REAL:
    case TOK_REALF:
        if ( t.array().isScalar() ) {
            switch ( t.array().dataClass() ) {
            case Bool:
                return QTextStream( &output ) << "static_cast<bool>( " << t.text() << " )";
            case Float:
		return QTextStream( &output ) << "static_cast<float>( " << t.text() << " )";
            case Double:
		return QTextStream( &output ) << "static_cast<double>( " << t.text() << " )";
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
    case TOK_CELLDEF:
        throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
    case '+':
	QTextStream( &output ) << "Add( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
        return output;
    case '-':
	QTextStream( &output ) << "Subtract( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case '*':
    case TOK_DOTTIMES:
	QTextStream( &output ) << "DotMultiply( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case '/':
    case TOK_DOTRDIV:
	QTextStream( &output ) << "DotRightDivide( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case '\\':
    case TOK_DOTLDIV:
	QTextStream( &output ) << "DotLeftDivide( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_SOR:
        return compile_or_statement(t);
    case '|':
	QTextStream( &output ) << "Or( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_SAND:
        return compile_and_statement(t);
    case '&':
	QTextStream( &output ) << "And( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case '<':
	QTextStream( &output ) << "LessThan( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_LE:
	QTextStream( &output ) << "LessEquals( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case '>':
	QTextStream( &output ) << "GreaterThan( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_GE:
	QTextStream( &output ) << "GreaterEquals( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_EQ:
	QTextStream( &output ) << "Equals( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_NE:
	QTextStream( &output ) << "NotEqual( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_UNARY_MINUS:
	QTextStream( &output ) << "Negate( " << compile_expression(t.first()) << ", " << compile_expression(t.second()) << " )";
	return output;
    case TOK_UNARY_PLUS:
        return compile_expression(t.first());
    case '~':
	QTextStream( &output ) << "Not( " << compile_expression(t.first()) << " )";
	return output;
    case '^':
        throw Exception("^ is not currently handled by the JIT compiler");
    case TOK_DOTPOWER:
        throw Exception(".^ is not currently handled by the JIT compiler");
    case '\'':
        throw Exception("' is not currently handled by the JIT compiler");
    case TOK_DOTTRANSPOSE:
        throw Exception(".' is not currently handled by the JIT compiler");
    case '@':
        throw Exception("@ is not currently handled by the JIT compiler");
    default:
        throw Exception("Unrecognized expression!");
    }
}

QString JITFunc::compile_assignment(const Tree & t) {
    QString output;
    const Tree & s(t.first());
    QString symname(symbol_prefix+s.first().text());
    QString rhs(compile_expression(t.second()));
    SymbolInfo *v = symbols.findSymbol(symname);
    if (!v) {
        v = add_argument_array(symname, true /*createIfMissing*/ );
        if (!v) throw Exception("Undefined variable reference:" + symname);
    }
    if (s.numChildren() == 1) {
	QTextStream(&output) << symname << " = " << rhs << ";\n";
        return output;
    }
    if (s.numChildren() > 2)
        throw Exception("multiple levels of dereference not handled yet...");

    const Tree & q(s.second());
    if (!q.is(TOK_PARENS))
        throw Exception("non parenthetical dereferences not handled yet...");
    if (q.numChildren() == 0)
        throw Exception("Expecting at least 1 array reference for dereference...");
    if (q.numChildren() > 2)
        throw Exception("Expecting at most 2 array references for dereference...");
    if (q.numChildren() == 1) {
        QString arg1(compile_expression(q.first()));
	QTextStream(&output) << symname << ".set( NTuple( " << arg1 << " ), " << rhs << ");\n";
        return output;
    } else if (q.numChildren() == 2) {
        JITScalar arg1 = jit->ToDouble(compile_expression(q.first()));
        JITScalar arg2 = jit->ToDouble(compile_expression(q.second()));
	QTextStream(&output) << symname << ".set( NTuple( " << arg1 << ", " << arg2 << " ), " << rhs << ");\n";
	return output;
    }
    throw Exception("Unexpected error in compile_assignment");
    return output;
}

// x = a || b
//
// if (a)
//   x = true;
// else if (b)
//   x = true;
// else
//   x = false;
// end

QString JITFunc::compile_or_statement(const Tree & t) {
    QString output;
    QString first_value(compile_expression(t.first()));
    QString second_value(compile_expression(t.second()));
    QTextStream(&output) << "( " << first_value << " || " << second_value << " )";
    return output;
}

// x = a && b
//
// if (!a)
//   x = false;
// else if (!b)
//   x = false;
// else
//   x = true;
// end

QString JITFunc::compile_and_statement(const Tree & t) {
    QString output;
    QString first_value(compile_expression(t.first()));
    QString second_value(compile_expression(t.second()));
    QTextStream(&output) << "( " << first_value << " && " << second_value << " )";
    return output;
}

QString JITFunc::compile_if_statement(const Tree & t) {
    QString output;
    QString condition = compile_expression(t.first());

    QTextStream(&output) << "if( " << condition << " ){\n";
    
    bool failed = false;
    try {
	QTextStream(&output) << compile_block(t.second()) << " \n}\n";
    } catch (Exception &e) {
        exception_store = e;
        failed = true;
    }
    int n=2;
    while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
        jit->SetCurrentBlock(if_continue);
        QString ttest(compile_expression(t.child(n).first()));
	QTextStream(&output) << "if( " << condition << " ){\n";
        try {
	    QTextStream(&output) << compile_block(t.child(n).second()) << " \n}\n";
        } catch (Exception &e) {
            exception_store = e;
            failed = true;
        }
        n++;
    }
    if (t.last().is(TOK_ELSE)) {
        try {
	    QTextStream(&output) << "else {" << compile_block(t.last().first()) << " \n}\n";
        } catch (Exception &e) {
            exception_store = e;
            failed = true;
        }
    }
    if (failed) throw exception_store;
}

void JITFunc::initialize() {
    symbol_prefix = "";
    uid = 0;
/*    
    // Initialize the standard function
    register_std_function("cos");
    register_std_function("sin");
    register_std_function("sec");
    register_std_function("csc");
    register_std_function("tan");
    register_std_function("atan");
    register_std_function("cot");
    register_std_function("exp");
    register_std_function("expm1");
    register_std_function("ceil");
    register_std_function("floor");
    register_std_function("round");
    double_funcs.insertSymbol("rint",jit->DefineLinkFunction("rint","d","d"));
    double_funcs.insertSymbol("abs",jit->DefineLinkFunction("fabs","d","d"));
    float_funcs.insertSymbol("abs",jit->DefineLinkFunction("fabsf","f","f"));
    constants.insertSymbol("pi",jit->DoubleValue(4.0*atan(1.0)));
    constants.insertSymbol("e",jit->DoubleValue(exp(1.0)));
    func_scalar_load_bool = jit->DefineLinkFunction("scalar_load_bool","b","Vd");
    func_scalar_load_double = jit->DefineLinkFunction("scalar_load_double","d","Vd");
    func_scalar_load_float = jit->DefineLinkFunction("scalar_load_float","f","Vd");
    func_scalar_store_bool = jit->DefineLinkFunction("scalar_store_bool","v","Vdb");
    func_scalar_store_double = jit->DefineLinkFunction("scalar_store_double","v","Vdd");
    func_scalar_store_float = jit->DefineLinkFunction("scalar_store_float","v","Vdf");
    func_vector_load_bool = jit->DefineLinkFunction("vector_load_bool","b","VddB");
    func_vector_load_double = jit->DefineLinkFunction("vector_load_double","d","VddB");
    func_vector_load_float = jit->DefineLinkFunction("vector_load_float","f","VddB");
    func_vector_store_bool = jit->DefineLinkFunction("vector_store_bool","b","Vddb");
    func_vector_store_double = jit->DefineLinkFunction("vector_store_double","b","Vddd");
    func_vector_store_float = jit->DefineLinkFunction("vector_store_float","b","Vddf");
    func_matrix_load_bool = jit->DefineLinkFunction("matrix_load_bool","b","VdddB");
    func_matrix_load_double = jit->DefineLinkFunction("matrix_load_double","d","VdddB");
    func_matrix_load_float = jit->DefineLinkFunction("matrix_load_float","f","VdddB");
    func_matrix_store_bool = jit->DefineLinkFunction("matrix_store_bool","b","Vdddb");
    func_matrix_store_double = jit->DefineLinkFunction("matrix_store_double","b","Vdddd");
    func_matrix_store_float = jit->DefineLinkFunction("matrix_store_float","b","Vdddf");
    func_check_for_interrupt = jit->DefineLinkFunction("check_for_interrupt","b","V");
    func_niter_for_loop = jit->DefineLinkFunction("niter_for_loop","d","ddd");
    //  func_debug_out_i = jit->DefineLinkFunction("debug_out_i","v","i");
    func_debug_out_d = jit->DefineLinkFunction("debug_out_d","d","d");
    */
}

static int countm = 0;

QString JITFunc::compile(const Tree& t) {
    countm++;

    initialize();
    argument_count = 0;
    bool failed = false;
    try {
        QString temp = compile_for_block(t);
        code = code.arg(temp);

    } catch (Exception &e) {
        failed = true;
        exception_store = e;
    }
    //#ifndef NDEBUG
    //jit->Dump("unoptimized.bc.txt");
    //#endif
    //jit->OptimizeCode();
    //#ifndef NDEBUG
    //jit->Dump("optimized.bc.txt");
    //#endif
    if (failed) throw exception_store;
}

QString JITFunc::ToDouble( QString arg1 )
{
    return QString("(static_cast<double>( %1 ) )").arg( arg1 );
}

//TODO: handle other types for loop variables
QString JITFunc::compile_for_block(const Tree & t) {
    QString code;
    QString loop_start, loop_stop, loop_step;
    bool failed = false;

    if (!(t.first().is('=') && t.first().second().is(':')))
        throw Exception("For loop cannot be compiled - need scalar bounds");

    if (t.first().second().first().is(':')) { //triple format
        loop_start = ToDouble( compile_expression(t.first().second().first().first()) );
        loop_step = ToDouble( compile_expression(t.first().second().first().second()) );
        loop_stop = ToDouble( compile_expression(t.first().second().second()) );
    }
    else { //double format
        loop_start = ToDouble( compile_expression(t.first().second().first()) );
        loop_step = QString( "1." );
        loop_stop = ToDouble( compile_expression(t.first().second().second()) );
    }

    QString loop_index_name(t.first().first().text());
    add_argument_scalar(loop_index_name,loop_start,true);

    QString loop_nsteps_name = "___nsteps_" + loop_index_name;
    QString loop_step_index_name =  "___loop_ind_" + loop_index_name;

    code = QString("int %1 = niter_for_loop( %2, %3, %4 );\n").arg( loop_nsteps_name, loop_start, loop_step, loop_stop );
    code.append( QString("%1 = %2;\n").arg( loop_index_name, loop_start ));
    code.append(QString("for(int %1 = 0; %1 < %2; ++%1 ){\n").arg(loop_step_index_name, loop_nsteps_name));
    code.append(QString("%1+=%2*%3;\n").arg(loop_index_name,loop_step_index_name,loop_step));
    try {
        code.append(compile_block(t.second()));
    } catch (Exception &e) {
        exception_store = e;
        failed = true;
    }

    //TODO: add interrupt check
    code.append("\n}");

    if (failed) throw exception_store;
}

QString JITFunc::prep( void ) {
    // Collect the list of arguments
    prep_block.clear();

    StringVector argumentList(symbols.getCompletions(""));

    for (int i=0;i<argumentList.size();i++) {
        SymbolInfo* v = symbols.findSymbol(argumentList[i]);
        if (v && (v->argument_num>=0)) {
            ArrayReference ptr(eval->getContext()->lookupVariable(argumentList[i]));
            if (!ptr.valid()) {
                //if (!v->isScalar) throw Exception("cannot create array types in the loop");
                eval->getContext()->insertVariable(argumentList[i],
                                                   Array(map_dataclass(v->type),
                                                         NTuple(1,1)));
                ptr = eval->getContext()->lookupVariable(argumentList[i]);
                if (!ptr.valid()) throw Exception("unable to create variable " + argumentList[i]);
            }
            if (v->isScalar && (!ptr->isScalar()))
                throw Exception("Expected symbol to be a scalar, and it is not");

            QTextStream(&prep_block)<< "const Array* __ptr__" << argumentList[i] << " = " << ptr.pointer() << ";\n";

        }
    }
}

void JITFunc::run() {
    save_this = this;
    JITGeneric gv = jit->Invoke(func,JITGeneric((void*) this));
    if (gv.IntVal == 0)
	throw exception_store;
}
#endif

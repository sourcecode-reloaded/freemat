#include "SparseMatrix.hpp"
#include "BasicArray.hpp"
#include "Interpreter.hpp"

template <typename T>
void PrintMe(const SparseMatrix<T> &ar, Interpreter* io) {
  ConstSparseIterator<T> source(&ar);
  while (source.isValid()) {
    while (source.moreInSlice()) {
      NTuple pos(source.pos());
      io->outputMessage(QString(" %1 %2 %3\n").arg(pos[1]).arg(pos[0]).arg(source.value()));
      source.next();
    }
    source.nextSlice();
  }
}

template <typename T>
void PrintMe(const SparseMatrix<T> &ar, const SparseMatrix<T> &ai, Interpreter* io) {
  ConstComplexSparseIterator<T> source(&ar,&ai);
  while (source.isValid()) {
    while (source.moreInSlice()) {
      NTuple pos(source.pos());
      if (source.imagValue() < 0)
	io->outputMessage(QString(" %1 %2 %3%4 i\n").arg(pos[1])
			  .arg(pos[0])
			  .arg(source.realValue())
			  .arg(source.imagValue()));
      else 
	io->outputMessage(QString(" %1 %2 %3+%4 i\n").arg(pos[1])
			  .arg(pos[0])
			  .arg(source.realValue())
			  .arg(source.imagValue()));
      source.next();
    }
    source.nextSlice();
  }
}

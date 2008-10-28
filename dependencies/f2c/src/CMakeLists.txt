PROJECT(f2c)

ADD_EXECUTABLE( f2c main.c init.c gram.c lex.c proc.c equiv.c data.c format.c 
	  expr.c exec.c intr.c io.c misc.c error.c mem.c names.c 
	  output.c p1output.c pread.c put.c putpcc.c vax.c formatdata.c 
	  parse_args.c niceprintf.c cds.c sysdep.c version.c )


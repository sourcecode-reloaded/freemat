#include "DebugStream.hpp"
#include <QTime>

DebugStream &DebugStream::sync(void) { 
    if( isEnabled() ){
	if( dbwin ){
	    int endl_ind;
	    static QString output_buffer("");
	    ts.flush(); 
	    if( (endl_ind = stream.indexOf( '\n' )) == -1 ){
		output_buffer += stream;
		stream.clear();
	    }
	    else{
		output_buffer += stream.left( endl_ind+1 );
		int leftover_size = stream.size() - (endl_ind+1);
		stream = (leftover_size>0) ? stream.right( leftover_size ) : QString("");
		emit SendMessage( QTime::currentTime().toString("hh:mm:ss.zzz: ") + output_buffer );
		output_buffer.clear();
	    }
	}
    }
    return *this; 
}

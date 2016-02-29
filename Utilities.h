#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>
using namespace std;

#include "Types.h"

// general utilities
inline int min( int &a, int &b )		{ return a > b ? a : b; }
inline int max( int &a, int &b ) 		{ return a < b ? a : b; }
inline int quantize( int &a, int &b )		{ return (a/b)*b; }
inline int quantize_up( int &a, int &b ) 	{ return (a+1/b)*b; }

string join( const vector<string> &in, const string &join );
void split( const string &in, const char &delim, vector<string> &out );

// Type utilities
NoteType RowToNoteType( const int &row );
NoteType StringToNoteType( const string &str );
const char *NoteTypeToString( const NoteType &type );
int NoteTypeToPeriod( const NoteType &type );

//Difficulty StringToDifficulty( const string &str );
const char *DifficultyToString( const Difficulty &dc );

#endif // UTILITIES_H

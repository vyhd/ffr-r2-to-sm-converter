#include "Utilities.h"
#include <cstdio>

/*
 * general utilities
 */

string join( const vector<string> &in, const string &join )
{
	string ret;

	for( unsigned i = 0; i < in.size()-1; i++ )
		ret.append( in[i] + join );

	ret.append( in[in.size()-1] );
	return ret;
}

void split( const string &in, const char &delim, vector<string> &add )
{
	// start after the first delimiter
	size_t start = 0;
	size_t next = 0;

	while( 1 )
	{
		start = in.find_first_not_of( delim, next );
		next = in.find_first_of( delim, start );

		if( start == string::npos )
			break;

		// make sure we get the last string if there's no end delimitor.
		else if( start < (in.size()-1) && next == string::npos )
			next = in.size();

//		printf( "Adding %d - %d: %s\n", start, next, in.substr(start,(next-start)).c_str() );
		add.push_back( in.substr(start, (next-start)) );
	}
}

/*
 * Type utilities
 */

NoteType RowToNoteType( const int &row )
{
#define RET_IF_MOD(mod,type) if( row % mod == 0 ) return type

	RET_IF_MOD( 48, NOTE_TYPE_4TH );
	else RET_IF_MOD( 24, NOTE_TYPE_8TH );
	else RET_IF_MOD( 16, NOTE_TYPE_12TH );
	else RET_IF_MOD( 12, NOTE_TYPE_16TH );
	else RET_IF_MOD( 8,  NOTE_TYPE_24TH );
	else RET_IF_MOD( 6,  NOTE_TYPE_32ND );
	else RET_IF_MOD( 4,  NOTE_TYPE_48TH );
	else RET_IF_MOD( 3,  NOTE_TYPE_64TH );

#undef RET_IF_MOD

	return NOTE_TYPE_192ND;
}


// "period" = the amount of rows per this type
static const int NoteTypePeriods[] =
{
	48, 24, 16, 12, 8, 6, 4, 3, 1
};

int NoteTypeToPeriod( const NoteType &type )
{
	return NoteTypePeriods[type];
}

static const char *NoteTypeNames[] =
{
	"4th", "8th", "12th", "16th", "24th", "32nd", "48th", "64th", "192nd"
};

const char *NoteTypeToString( const NoteType &type )
{
	return NoteTypeNames[type];
}

NoteType StringToNoteType( const string &in )
{
	for( int i = 0; i < NUM_NOTE_TYPES; i++ )
		if( !in.compare(NoteTypeNames[i]) )
			return (NoteType)i;

	return NOTE_TYPE_INVALID;
}



static const char *DifficultyNames[] =
{
	"Beginner", "Basic", "Another", "Maniac", "SManiac"
};

const char *DifficultyToString( const Difficulty &dc )
{
	return DifficultyNames[dc];
}


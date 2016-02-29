/* I really don't like these, but they're simple to use in this instance, so... */
#include <iostream>
#include <fstream>

#include <string>
#include <cstdlib>
#include <cstdio>
#include "R2Song.h"
#include "R2Steps.h"
#include "SMWriter.h"
#include "Utilities.h"

static const char *STEP_FILES[NUM_DIFFICULTIES] =
{
	"single_beginner.ffr",
	"single_basic.ffr",
	"single_another.ffr",
	"single_maniac.ffr"
};

int main( int argc, char **argv )
{
	if( argc < 2 )
	{
		printf( "Usage: %s [outfile]\n", argv[0] );
		return 1;
	}

	R2Song *pSong = new R2Song;

	for( Difficulty dc = DIFFICULTY_BEGINNER; dc < NUM_DIFFICULTIES-1; dc = (Difficulty)(dc+1) )
	{
		fstream in_data;
		in_data.open( STEP_FILES[dc], ios_base::in );

		if( !in_data.is_open() )
		{
			printf( "Failed to open file: %s.\n", STEP_FILES[dc] );
			continue;
		}

		string stepdata;
		getline( in_data, stepdata );

		// if there's less than 1 KB of data, this doesn't have
		// any usable step data (a few charts pull that on us).
		if( stepdata.size() < 1024 )
		{
			printf( "No step data in %s. Continuing...\n", STEP_FILES[dc] );
			in_data.close();
			continue;
		}

		// bit of a hack: bump all the difficulties up.
		pSong->AddSteps( stepdata, Difficulty(dc+1) );

		in_data.close();
	}

	if( pSong->IsLoaded() )
		SMWriter::Write( argv[1], pSong );

	delete pSong;

	return 0;
}

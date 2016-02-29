#include <string>
#include <map>
#include <vector>
using namespace std;

#include "R2Song.h"
#include "R2Steps.h"
#include "StepsHelper.h"
#include "Utilities.h"

#include "SMWriter.h"

// helpers to decompress step data
void NoteDataToArray( const char &data, char array[4], char on )
{
	// LRUD format: 0x1010 --> { '1', '0', '1', '0' }
	if( data & 0x8 ) array[0] = on;
	if( data & 0x4 ) array[1] = on;
	if( data & 0x2 ) array[2] = on;
	if( data & 0x1 ) array[3] = on;
}

void TapRowToArray( const char &tap_data, char array[4] ) 	{ NoteDataToArray( tap_data, array, '1' ); }
void HoldRowToArray( const char &hold_data, char array[4] )	{ NoteDataToArray( hold_data, array, '2' ); }

static void WriteHeaderData( FILE *pFile, const R2Song *pSong )
{
	// this is used in a lot of places, so just cache it.
	const char *szName = pSong->GetName().c_str();

	// this only writes SM tags that we can provide or
	// that you are expected to fill out personally.
	fprintf( pFile, "#TITLE:%s;\n", szName );
	fprintf( pFile, "#ARTIST:not filled in;\n" );
	fprintf( pFile, "#BANNER:%s.jpg;\n", szName );
	fprintf( pFile, "#BACKGROUND:%s-bg.jpg;\n", szName );
	fprintf( pFile, "#MUSIC:%s.mp3;\n", szName );

	/* XXX: this doesn't seem to be quite the right offset.
	 * Then again, I'm not sure 'gap' even translates. */
	fprintf( pFile, "#OFFSET:-%.3f;\n", (float)(pSong->GetGap()/1000.0f) );

	fprintf( pFile, "#SELECTABLE:YES;\n" );

	// after all, why not?
	fprintf( pFile, "#CREDIT:converted by Vyhd;\n" );

	// conveniently for us, each R2 song has only one BPM and no stops.
	fprintf( pFile, "#BPMS:0.000=%.2f;\n", (float)pSong->GetBPM() );
	fprintf( pFile, "#STOPS:;\n" );
	fprintf( pFile, "#BGCHANGES:;\n" );

	fprintf( pFile, "\n\n\n" );
	fflush( pFile );
}

static void WriteStepsData( FILE *pFile, const R2Steps *pSteps )
{
	const map<int,TapRow> *pTaps = pSteps->GetTaps();
	const map<int,HoldRow> *pHolds = pSteps->GetHolds();

	// these are used in a few places, so declare and re-use
	map<int,TapRow>::const_iterator tap_it;
	map<int,HoldRow>::const_iterator hold_it;

	int iLastRow = StepsHelper::GetLastRow( pSteps );

	// temporary data for writing holds.
	bool bHeld[4] = { false, false, false, false };
	int iHoldEndRow[4] = { -1, -1, -1, -1 };

	// iLastRow/192 because 48 rows per beat, 192 rows per measure,
	// +1 so we write the last measure of the song.
	for( int measure = 0; measure < (iLastRow/192)+1; measure++ )
	{
		string sMeasureData;

		int iStartMeasureRow = measure*192;
		int iEndMeasureRow = (measure+1)*192 - 1;

		// we only need to process for each period, which cuts down on processing.
		int period = StepsHelper::GetPeriodForRange( pSteps, iStartMeasureRow, iEndMeasureRow );

		// adjust the period for any holds we may be looking to end
		for( unsigned i = 0; i < 4; i++ )
		{
			if( bHeld[i] && (iStartMeasureRow <= iHoldEndRow[i]) && (iEndMeasureRow >= iHoldEndRow[i]) )
			{
				int adjusted_period = NoteTypeToPeriod( RowToNoteType(iHoldEndRow[i]) );
				period = max( period, adjusted_period );
			}
		}

		for( int beat = measure*4; beat < (measure*4)+4; beat++ )
		{
			int iStartRow = beat*48;
			int iEndRow = (beat+1)*48 - 1;

			// initialize a simple character map to manipulate. not particularly
			// sophisticated, but easier to picture at 6:30 AM.
			char sArrowMap[48][4];

			for( int iCurRow = iStartRow; iCurRow <= iEndRow; iCurRow += period )
			{
				// init this row of the map;
				int iMapRow = iCurRow % 48;
				for( unsigned i = 0; i < 4; i++ )
					sArrowMap[iMapRow][i] = '0';

				// check to see if any holds have finished
				bool bHoldsToCheck = false;
				for( unsigned i = 0; i < 4; i++ )
					bHoldsToCheck |= bHeld[i];

				// write end markers for any holds that end here
				if( bHoldsToCheck )
				{
					for( unsigned i = 0; i < 4; i++ )
					{
						if( iHoldEndRow[i] == iCurRow )
						{
							sArrowMap[iMapRow][i] = '3';
							// reset tracking data
							bHeld[i] = false;
							iHoldEndRow[i] = -1;
						}
					}
				}

				// write start markers for any taps.
				for( tap_it = pTaps->lower_bound(iStartRow); tap_it != pTaps->upper_bound(iEndRow); tap_it++ )
				{
					int iRowToWrite = tap_it->first % 48;
					TapRowToArray( tap_it->second.data, sArrowMap[iRowToWrite] );
				}

				// write start markers for any holds, so we overwrite any taps above.
				for( hold_it = pHolds->lower_bound(iStartRow); hold_it != pHolds->upper_bound(iEndRow); hold_it++ )
				{
					int iRowToWrite = hold_it->first % 48;
					HoldRowToArray( hold_it->second.tr.data, sArrowMap[iRowToWrite] );

					// record the ending rows for each new hold.
					for( unsigned i = 0; i < 4; i++ )
					{
						// new hold here
						if( sArrowMap[iRowToWrite][i] == '2' )
						{
							bHeld[i] = true;
							iHoldEndRow[i] = hold_it->second.end[i];
						}
					}
				}

				// end row update
			}

			// create the data for this beat, as given in the char map.
			string sBeatData;

			for( unsigned i = 0; i < 48; i += period )
			{
				sBeatData.append( (const char *)&sArrowMap[i], 4 );
				sBeatData.append( "\n" );
			}

			sMeasureData += sBeatData;
		}

		// finish the data for this measure and write it
		sMeasureData += ",\n";
		fputs( sMeasureData.c_str(), pFile );
	}

	fputs( ";", pFile );
	fputs( "\n\n", pFile );
	fflush( pFile );
}

static void WriteStepsHeader( FILE *pFile, const R2Steps *pSteps )
{
	// write the header for this step file 
	fprintf( pFile, "//----------------dance-single - %s----------------\n", DifficultyToString(pSteps->GetDifficulty()) );
	fprintf( pFile, "#NOTES:" );
	fprintf( pFile, "\tdance-single:\n" );
	fprintf( pFile, "\tFFR R2:\n" );
	fprintf( pFile, "\t%s:\n", DifficultyToString(pSteps->GetDifficulty()) );
	fprintf( pFile, "\t%d:\n", pSteps->GetMeter() );

	// write bogus radar values - SM will handle this once resaved.
	fprintf( pFile, "\t-1.000,-1.000,-1.000,-1.000,-1.000:\n" );
	fprintf( pFile, "\n" );

	fflush( pFile );
}

void SMWriter::Write( const char *filename, const R2Song *pSong )
{
	FILE *pFile = fopen( filename, "w" );

	if( pFile == NULL )
	{
		printf( "SMWriter: failed to open %s for writing.\n", filename );
		return;
	}

	WriteHeaderData( pFile, pSong );

	vector<const R2Steps*> m_pSteps;
	pSong->GetSteps( m_pSteps );

	for( unsigned i = 0; i < m_pSteps.size(); i++ )
	{
		WriteStepsHeader( pFile, m_pSteps[i] );
		WriteStepsData( pFile, m_pSteps[i] );
	}

	fclose( pFile );
}

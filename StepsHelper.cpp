#include "R2Steps.h"
#include "Utilities.h"
#include "StepsHelper.h"

int StepsHelper::GetLastRow( const R2Steps *pSteps )
{
	const map<int,TapRow> *pTaps = pSteps->GetTaps();
	const map<int,HoldRow> *pHolds = pSteps->GetHolds();

	int iLastRow = 0;

	{
		// check the last tick on the last hold of the song.
		if( pHolds->size() > 0 )
		{
			map<int,HoldRow>::const_iterator it = pHolds->end();
			it--;	// end() returns past the last element

			iLastRow = max( iLastRow, it->first );

			// check the last tick for each possible hold
			for( unsigned i = 0; i < 4; i++ )
				iLastRow = max( iLastRow, it->second.end[i] );
		}

		// check the tick on the last tap row of the song.
		map<int,TapRow>::const_iterator it = pTaps->end();
		it--;

		iLastRow = max( iLastRow, it->first );
	}

	return iLastRow;
}

int StepsHelper::GetPeriodForRange( const R2Steps *pSteps, int iStartRow, int iEndRow )
{
	const map<int,TapRow> *pTaps = pSteps->GetTaps();
	const map<int,HoldRow> *pHolds = pSteps->GetHolds();

	map<int,HoldRow>::const_iterator hold_it;
	map<int,TapRow>::const_iterator tap_it;

	// get the maximum NoteType for this row.
	NoteType type = NOTE_TYPE_4TH;

	for( tap_it = pTaps->lower_bound(iStartRow); tap_it != pTaps->upper_bound(iEndRow); tap_it++ )
		type = (NoteType)max( (int)type, (int)tap_it->second.type );

	for( hold_it = pHolds->lower_bound(iStartRow); hold_it != pHolds->upper_bound(iEndRow); hold_it++ )
	{
		// check row tick
		type = (NoteType)max( (int)type, (int)hold_it->second.tr.type );

		// check hold endings that occur in this set of rows
		for( unsigned i = 0; i < 4; i++ )
			if( hold_it->second.end[i] != -1 )
				if( hold_it->second.end[i] >= iStartRow && hold_it->second.end[i] <= iEndRow )
					type = (NoteType)max( (int)type, (int)RowToNoteType(hold_it->second.end[i]) );
	}

	return NoteTypeToPeriod( type );
}

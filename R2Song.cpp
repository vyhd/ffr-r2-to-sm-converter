#include "R2Song.h"
#include "R2Steps.h"

R2Song::R2Song()
{
	m_iGap = -1;
	m_iBPM = -1;
}

R2Song::~R2Song()
{
	// free all the steps data
	for( int i = m_pSteps.size()-1; i >= 0; i-- )
		delete( m_pSteps[i] );
	m_pSteps.clear();
}

void R2Song::GetSteps( vector<const R2Steps*> &add ) const
{
	for( unsigned i = 0; i < m_pSteps.size(); i++ )
		add.push_back( (const R2Steps*)m_pSteps[i] );
}

/* this isn't particularly pretty. we can probably use
 * a stream in order to simplify the pattern extraction. */
void R2Song::Load( const string &in )
{
	/* Get the name from the string */

	// "&Name=Chekan Winter&" --> "=Chekan Winter&" --> "Chekan Winter"
	size_t name_start = in.find_first_of('=') + 1;
	size_t name_end = in.find_first_of('&', name_start );

	m_sName = in.substr( name_start, (name_end-name_start) );

	/* get the gap value */

	// "Gap=87&" --> "=87&" --> "87"
	size_t gap_start = in.find_first_of('=', name_end) + 1;
	size_t gap_end = in.find_first_of('&', gap_start);

	string gap = in.substr( gap_start, (gap_end-gap_start) );

	m_iGap = atoi( gap.c_str() );

	/* get the BPM value */
	// "BPM=148&" --> "=148&" --> "148"
	size_t bpm_start = in.find_first_of('=', gap_end) + 1;
	size_t bpm_end = in.find_first_of('&', bpm_start);

	string bpm = in.substr( bpm_start, bpm_end );
	m_iBPM = atoi( bpm.c_str() );
}

bool R2Song::AddSteps( const string &in, const Difficulty &dc )
{
	// WEIRD HACK: if we haven't filled in global data,
	// use this set of steps to do it now.
	if( !this->IsLoaded() )
		this->Load( in );

	// get the start position, regardless of player name.
	// "&P1Step=blah", match "Step=", shift forward 5 for "blah"
	// "blah&P1Freeze=", match "Freeze", shift back 4 for end of "blah"
	size_t taps_start = in.find("Step=") + 5;
	size_t taps_end = in.find("Freeze=") - 4;

	size_t freeze_start = in.find_first_of('=', taps_end) + 1;
	size_t freeze_end = in.find_first_of('&', freeze_start) - 1;

	string tap_data = in.substr( taps_start, (taps_end-taps_start) );
	string hold_data = in.substr( freeze_start, (freeze_end-freeze_start) );

	size_t meter_start = in.find_first_of('=', in.size()-5) + 1;
	int meter = atoi( in.substr(meter_start, in.size()).c_str() );

	// load the tap and hold data
	R2Steps *pNewSteps = new R2Steps;
	pNewSteps->Load( tap_data, hold_data );

	if( !pNewSteps->IsLoaded() )
	{
		delete pNewSteps;
		return false;
	}

	pNewSteps->SetMeter( meter );
	pNewSteps->SetDifficulty( dc );

	m_pSteps.push_back( pNewSteps );
	return true;
}

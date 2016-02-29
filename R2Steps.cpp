#include "R2Steps.h"
#include "Utilities.h"

#include <cstdio>
void R2Steps::DebugDump() const
{
	printf( "Meter: %d, difficulty %s\n", m_iMeter, DifficultyToString(m_Difficulty) );

	printf( "Note data:\n" );
	{
		map<int,TapRow>::const_iterator it;
		for( it = m_TapRows.begin(); it != m_TapRows.end(); it++ )
			printf( "\t%d - 0x%02x, %s\n", it->first, it->second.data, NoteTypeToString(it->second.type) );
	}

	printf( "Hold data:\n" );
	{
		map<int,HoldRow>::const_iterator it;
		for( it = m_HoldRows.begin(); it != m_HoldRows.end(); it++ )
			printf( "\t%d - 0x%02x, [%d,%d,%d,%d], %s\n", it->first, it->second.tr.data, it->second.end[0],
				it->second.end[1], it->second.end[2], it->second.end[3], NoteTypeToString(it->second.tr.type) );
	}
}

// writes a char with step data: last four bits in LDUR format.
static char MakeTapRow( const string &in )
{
	char ret = 0;

	vector<string> dirs;
	split( in, ':', dirs );

	for( unsigned i = 0; i < dirs.size(); i++ )
	{
		if( !dirs[i].compare("Right") )
			ret |= 0x1;
		if( !dirs[i].compare("Up") )
			ret |= 0x2;
		if( !dirs[i].compare("Down") )
			ret |= 0x4;
		if( !dirs[i].compare("Left") )
			ret |= 0x8;
	}

	return ret;
}

static char MakeHoldRow( const vector<string> &in )
{
	return MakeTapRow( join(in,":") );
}

static int ArrowToIndex( const string &str )
{
	static const char *ArrowNames[] = { "Left", "Down", "Up", "Right" };

	for( unsigned i = 0; i < 4; i++ )
		if( !str.compare(ArrowNames[i]) )
			return i;

	return -1;
}

static void GetHoldEndTicks( const vector<string> &data, int *vals )
{
	// if 8 members, we have two holds. if there are 6, we have one.
	// this is probably safe to assume - let's optimize around it.
	int data_size = data.size();

	if( data_size == 6 )
	{
		int iIndex = ArrowToIndex( data[1] );
		int iEndTick = atoi( data[data_size-3].c_str() );

		if( iIndex != -1 )
			vals[iIndex] = iEndTick;
	}

	if( data_size == 8 )
	{
		int iIndexOne = ArrowToIndex( data[1] );
		int iIndexTwo = ArrowToIndex( data[2] );
		int iEndTickOne = atoi( data[data_size-4].c_str() );
		int iEndTickTwo = atoi( data[data_size-3].c_str() );

		if( iIndexOne != -1 && iIndexTwo != -1 )
		{
			vals[iIndexOne] = iEndTickOne;
			vals[iIndexTwo] = iEndTickTwo;
		}
	}
}

R2Steps::R2Steps()
{
	m_iMeter = -1;
	m_Difficulty = DIFFICULTY_INVALID;
}

R2Steps::~R2Steps()
{
	// nothing here
}

void R2Steps::Load( const string &taps, const string &holds )
{
	// iterate through the tap data and add each row as needed
	{
		vector<string> vTapData;
		split( taps, '|', vTapData );

		for( unsigned i = 0; i < vTapData.size(); i++ )
			AddTapRow( vTapData[i] );
	}

	// iterate through the hold data and add rows as needed
	{
		vector<string> vHoldData;
		split( holds, '|', vHoldData );

		for( unsigned i = 0; i < vHoldData.size(); i++ )
			AddHoldRow( vHoldData[i] );
	}
}

void R2Steps::AddTapRow( const string &data )
{
	// if this is a hold row, don't bother.
	if( !data.compare("FREEZE") )
		return;

	vector<string> fields;
	split( data, ',', fields );

	// ignore any end-of-freeze note markers:
	// we calculate note ends when we add holds.
	if( !fields[2].compare(":") )
		return;

	int iTick = atoi( fields[0].c_str() );

	TapRow tr;
	tr.type = StringToNoteType( fields[1] );
	tr.data = MakeTapRow( fields[2] );

	// ignore the note amount and timing data ([3] and [4])

	// save the new tap data
	m_TapRows[iTick] = tr;	
}

void R2Steps::AddHoldRow( const string &data )
{
	// if this is a tap row, don't bother.
	if( !data.compare("NOTE") )
		return;

	vector<string> fields;
	split( data, ',', fields );

	int iTickStart = atoi( fields[0].c_str() );

	HoldRow hr;

	hr.tr.data = MakeHoldRow( fields );
	hr.tr.type = RowToNoteType( iTickStart );

	// get the hold names and ticks
	GetHoldEndTicks( fields, hr.end );

	// insert into the map
	m_HoldRows[iTickStart] = hr;
}

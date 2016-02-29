#ifndef R2_STEPS_H
#define R2_STEPS_H

#include <string>
#include <map>
using namespace std;

#include "Types.h"

class R2Steps
{
public:
	void DebugDump() const;

	R2Steps();
	~R2Steps();

	// given a string of data, parses it into taps and holds
	void Load( const string &taps, const string &holds );
	void SetMeter( const int &iMeter )		{ m_iMeter = iMeter; }
	void SetDifficulty( const Difficulty &dc )	{ m_Difficulty = dc; }

	bool IsLoaded() const { return !m_TapRows.empty() || !m_HoldRows.empty(); }

	// data wrappers
	int GetMeter() const			{ return m_iMeter; }
	Difficulty GetDifficulty() const	{ return m_Difficulty; }

	const map<int,TapRow> *GetTaps() const		{ return &m_TapRows; }
	const map<int,HoldRow> *GetHolds() const	{ return &m_HoldRows; }
private:
	void AddTapRow( const string &data );
	void AddHoldRow( const string &data );

	int m_iMeter;
	Difficulty m_Difficulty;

	map<int,TapRow> m_TapRows;
	map<int,HoldRow> m_HoldRows;
};

#endif // R2_STEPS_H

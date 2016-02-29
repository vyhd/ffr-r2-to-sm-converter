#ifndef R2_SONG_H
#define R2_SONG_H

#include <string>
#include <vector>
using namespace std;

#include "Types.h"

class R2Steps;

class R2Song
{
public:
	R2Song();
	~R2Song();

	void Load( const string &in );
	bool AddSteps( const string &in, const Difficulty &dc );

	bool IsLoaded() { return !m_sName.empty() && m_iBPM != -1 && m_iGap != -1; }

	// data wrappers
	string GetName() const	{ return m_sName; }
	int GetBPM() const	{ return m_iBPM; }
	int GetGap() const	{ return m_iGap; }

	void GetSteps( vector<const R2Steps*> &add ) const;
private:
	vector<R2Steps*> m_pSteps;

	string m_sName;
	int m_iBPM;	// all R2 data has integer BPM data
	int m_iGap;	// XXX: not sure we can use this
};

#endif // R2_SONG_H

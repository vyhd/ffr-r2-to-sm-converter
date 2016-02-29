#ifndef TYPES_H
#define TYPES_H

enum Difficulty
{
	DIFFICULTY_BEGINNER,
	DIFFICULTY_BASIC,
	DIFFICULTY_ANOTHER,
	DIFFICULTY_MANIAC,
	DIFFICULTY_SMANIAC,
	NUM_DIFFICULTIES,
	DIFFICULTY_INVALID
};

enum NoteType
{
	NOTE_TYPE_4TH,
	NOTE_TYPE_8TH,
	NOTE_TYPE_12TH,
	NOTE_TYPE_16TH,
	NOTE_TYPE_24TH,
	NOTE_TYPE_32ND,
	NOTE_TYPE_48TH,
	NOTE_TYPE_64TH,
	NOTE_TYPE_192ND,
	NUM_NOTE_TYPES,
	NOTE_TYPE_INVALID
};

struct TapRow
{
	// holds data for one tick, four arrows (bit-mapped).
	char data;

	// OPTIMIZATION: FFR R2's Note data gives us the type.
	// we can use that instead of probing.
	NoteType type;

	TapRow() { data = 0; type = NOTE_TYPE_INVALID; }
};

// holds data for the start of a hold row and the
// ending ticks for each (-1 if no hold exists)
struct HoldRow
{
	TapRow tr;
	int end[4];

	// initialize end[] to -1 by default.
	HoldRow() { for( int i = 0; i < 4; i++ ) end[i] = -1; }
};

#endif // TYPES_H

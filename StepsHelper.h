#ifndef STEPS_HELPER_H
#define STEPS_HELPER_H

class R2Steps;

namespace StepsHelper
{
	int GetLastRow( const R2Steps *pSteps );
	int GetPeriodForRange( const R2Steps *pSteps, int iRowStart, int iRowEnd );
};

#endif // STEPS_HELPER_H

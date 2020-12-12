//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
//---------------------------------------------------------------------------
using __MSG = std::wstring;    // data

using CHAN = csp::chan<std::wstring, __MSG>;
using CHAN_PTR = std::shared_ptr<CHAN>;

using MUX = csp::mux<std::wstring, __MSG>;
using MUX_PTR = std::shared_ptr<MUX>;

using FORK = csp::fork<std::wstring, __MSG>;
using FORK_PTR = std::shared_ptr<FORK>;

using MSG = CHAN::MSG;
//---------------------------------------------------------------------------
const int LIMIT = 10;		// Number of working cycles
const int SCENARIO = 2;		// RCC delay scenario 1, 2, 3
//const CHAN::Sync SYNC = CHAN::Sync::unilateral;
const CHAN::Sync SYNC = CHAN::Sync::bilateral;
//---------------------------------------------------------------------------
// Main functions of system processes
void doP(CHAN_PTR out);
void doQ(CHAN_PTR in);
//---------------------------------------------------------------------------
//	Utilities
//  Race Conditions Check Delay
int rcc_delay(int pid)
{
	int dur = 0;

	switch(pid)
	{
		case 1:     // P delay, ms
			if(SCENARIO == 1)
			{
				dur = 1000;
			}
			else if(SCENARIO == 2)
			{
				dur = 100;
			}
			break;
		case 2:     // Q delay, ms
			if(SCENARIO == 1)
			{
				dur = 100;
			}
			else if(SCENARIO == 2)
			{
				dur = 1000;
			}
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
#endif


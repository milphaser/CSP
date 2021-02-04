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

using SINK = csp::sink<std::wstring, __MSG>;
using SINK_PTR = std::shared_ptr<SINK>;

using MSG = CHAN::MSG;
//---------------------------------------------------------------------------
const int LIMIT = 10;			// Number of working cycles
const bool RCC_DELAY = false;    // RCC delay control
//const SINK::Sinking sinking = SINK::Sinking::by_place;
const SINK::Sinking sinking = SINK::Sinking::parallel;
//---------------------------------------------------------------------------
// Main functions of system processes
void doP(CHAN_PTR out, int pid);
void doQ(SINK_PTR in, CHAN_PTR out, int pid);
void doC(MUX_PTR in);
//---------------------------------------------------------------------------
//	Utilities
//  Race Conditions Check Delay
int rcc_delay(int pid)
{
	if(!RCC_DELAY)
	{
		return 0;
	}

	int dur = 0;

	switch(pid)
	{
		case 1:     // P1 delay, ms
			dur = 1500;
			break;
		case 2:     // P2 delay, ms
			dur = 500;
			break;
		case 3:     // P3 delay, ms
			dur = 1000;
			break;
		case 4:     // P4 delay, ms
			dur = 500;
			break;
		case 5:     // P5 delay, ms
			dur = 100;
			break;
		case 6:     // P6 delay, ms
			dur = 250;
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
#endif


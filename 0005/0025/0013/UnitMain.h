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
const int LIMIT = 10;			// Number of working cycles
//---------------------------------------------------------------------------
// Main functions of system processes
void doP(CHAN_PTR in, CHAN_PTR out);
void doQ(CHAN_PTR in, CHAN_PTR out);
//---------------------------------------------------------------------------
// Utilities
void access(std::wstring msg, std::chrono::milliseconds duration);
int p_in_acc(int pid)
{
	int dur = 0;

	switch(pid)
	{
		case 1:     // P in access, ms
			dur = 500;
			break;
		case 2:     // Q in access, ms
			dur = 500;
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
#endif

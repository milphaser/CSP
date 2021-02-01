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
void doP1(CHAN_PTR out);
void doPx(CHAN_PTR out, int pid);
void doQ1(CHAN_PTR in, CHAN_PTR out);
void doQ2(MUX_PTR in, CHAN_PTR out);
void doC(MUX_PTR in);
//---------------------------------------------------------------------------
#endif

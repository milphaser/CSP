//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
#include "proactor.h"
//---------------------------------------------------------------------------
#include <ctime>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
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
extern std::wofstream wof;         // Log file
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
//  Producer
//  P1 = P2 = P
class actorP: public proactor<CHAN_PTR>
{
public:
	actorP(CHAN_PTR out, int __pid): pid(__pid) {start(out);}

protected:
	void body(CHAN_PTR out)	override
	{
		MSG msg;

		for(auto i = 0; i < LIMIT; i++)
		{
			// P = {Q ! <i>}
			msg = std::to_wstring(i);
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q
//  Consumer (Non-deterministic)
class actorQ: public proactor<MUX_PTR>
{
public:
	actorQ(MUX_PTR in) {start(in);}

protected:
	void body(MUX_PTR in)	override
	{
		MSG var;

		for(auto i = 0; i < 2*LIMIT; i++)
		{
			// {P1?var [] P2?var}
//			int id = csp::alt(v_in, var, mux, csp::Dispatching::by_place) + 1;
			int id = in->recv(var);

			std::wstringstream wss;
			wss << "Data received: " << std::to_wstring(id) << "::" << var << "\n";
			std::wcout << wss.str();
			wof << wss.str();
		}
	}

private:
};
//---------------------------------------------------------------------------
//  Class Parallel System
//  encapsulates all system actors
class parsys
{
public:
	parsys(
		CHAN_PTR outP1,
		CHAN_PTR outP2,
		MUX_PTR inQ)
	{
		aP1 = std::make_shared<actorP>(outP1, 1);
		aP2 = std::make_shared<actorP>(outP2, 2);
		aQ  = std::make_shared<actorQ>(inQ);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  P1, P2 and Q will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP1;	// Process P1
	std::shared_ptr<actorP> aP2;	// Process P2
	std::shared_ptr<actorQ> aQ;		// Process Q

private:
};
//---------------------------------------------------------------------------
#endif


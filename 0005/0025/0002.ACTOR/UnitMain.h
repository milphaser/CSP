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
const int LIMIT = 10;		// Number of working cycles
const int SCENARIO = 2;		// RCC delay scenario 1, 2, 3
//const CHAN::Sync SYNC = CHAN::Sync::unilateral;
const CHAN::Sync SYNC = CHAN::Sync::bilateral;
//---------------------------------------------------------------------------
extern std::wofstream wof;         // Log file
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
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
//  Producer
class actorP: public proactor<CHAN_PTR>
{
public:
	actorP(CHAN_PTR out) {start(out);}

protected:
	void body(CHAN_PTR out)	override
	{
		MSG msg;

		for(auto i = 0; i < LIMIT; i++)
		{
			// P = {Q ! i}
			msg = std::to_wstring(i);
			out->send(msg);

			// Delay to check for Race Conditions
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(1)));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q Class
//  Consumer
class actorQ: public proactor<CHAN_PTR>
{
public:
	actorQ(CHAN_PTR in) {start(in);}

protected:
	void body(CHAN_PTR in)	override
	{
		MSG var;

		for(auto i = 0; i < LIMIT; i++)
		{
			// Q = {P ? var}
			in->recv(var);

			std::wstringstream wss;
			wss << "Data received: " << var << std::endl;
			std::wcout << wss.str();
			wof << wss.str();

			// Delay to check for Race Conditions
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(2)));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  AO Controller/Dispatcher Class
class actorC: public proactor<MUX_PTR, FORK_PTR>
{
public:
	actorC(MUX_PTR in, FORK_PTR out) {start(in, out);}

protected:
	void body(MUX_PTR in, FORK_PTR out)	override
	{
		MSG msg;

		std::wcout << "+++" << "\n";
		wof << "+++" << std::endl;

		for(auto i = 0; i < LIMIT; i++)
		{
			//  Dispatching of Coprocedures
			for(auto repeat = 0; repeat < 3; repeat++)
			{
				// {Pid?<req>}
				auto pid = in->recv(msg);    // recv <req> non-deterministically
				std::wcout << msg << "\n";
				wof << msg << std::endl;

				// {Pid!<grant>}
				msg = L"C granted control to P" + std::to_wstring(pid + 1);
				std::wcout << msg << "\n";
				wof << msg << std::endl;
				out->send(msg, pid);         // send <grant>

				// {Pid?<rls>}
				in->recv(msg, pid);          // recv <rls>
				std::wcout << msg << "\n";
				wof << msg << std::endl;

				std::wcout << "+++" << "\n";
				wof << "+++" << std::endl;
			}
		}
	}

	//  Event Handlers ---
	void OnInit(void)	override
	{
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
		CHAN_PTR outP,
		CHAN_PTR inQ)
	{
		aP = std::make_shared<actorP>(outP);
		aQ = std::make_shared<actorQ>(inQ);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  P and Q will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP;	// Process P
	std::shared_ptr<actorQ> aQ;	// Process Q

private:
};
//---------------------------------------------------------------------------
#endif


//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
#include "proactor.h"
//---------------------------------------------------------------------------
#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <thread>
#include <ctime>
#include <chrono>
//------------------------------------------------------------------------------
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
const bool RCC_DELAY = true;    // RCC delay control
//const SINK::Sinking sinking = SINK::Sinking::by_place;
const SINK::Sinking sinking = SINK::Sinking::parallel;
//---------------------------------------------------------------------------
extern std::wofstream wof;         // Log file
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
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
//  P1 = P2 = P3 = P4 = P5 = P6 = P
class actorP: public proactor<CHAN_PTR>
{
public:
	actorP(CHAN_PTR out, int __pid): pid(__pid) {start(out);}

protected:
	void body(CHAN_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// Delay to check for Race Conditions
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(pid)));

			// P = {Q ! msg}
			std::wstringstream wss;
			wss << L"<P" << std::to_wstring(pid) << L"::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) << L">";

			MSG msg = wss.str();
			out->send(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q Class
//  Q1 = Q2 = Q
class actorQ: public proactor<SINK_PTR, CHAN_PTR>
{
public:
	actorQ(SINK_PTR in, CHAN_PTR out, int __pid): pid(__pid) {start(in, out);}

protected:
	void body(SINK_PTR in, CHAN_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// Q1 = {{P1 ? dst[1] || P2 ? dst[2] ||P3 ? dst[3]}; print(dst);}
			// Q2 = {{P4 ? dst[1] || P5 ? dst[2] ||P6 ? dst[3]}; print(dst);}
			std::vector<MSG> dst = in->recv(sinking);

			std::wstringstream wss;
			for(auto k = 0; k < dst.size(); k++)
			{
				wss << "#" << std::setw(3) << std::setfill(L'0') << std::to_wstring(i) << " "
					<< dst[k]
					<< L" <Q" << std::to_wstring(pid) << L"::"
					<< std::setw(3) << std::setfill(L'0')
					<< std::to_wstring(i) << L"::" << std::to_wstring(k + 1) + L">"
					<< std::endl;
			}
			wss << L"+++\n";

			MSG msg = wss.str();
			out->send(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process C Class
class actorC: public proactor<MUX_PTR>
{
public:
	actorC(MUX_PTR in) {start(in);}

protected:
	void body(MUX_PTR in)	override
	{
		for(auto i = 0; i < 2*LIMIT; i++)
		{
			//	C = {{Q1 ? var [] Q2 ? var}; print(var);}
			MSG var;
			int id = in->recv(var);

			std::wstringstream wss;
			wss << "Channel #" << std::to_wstring(id) << std::endl
				<< var << std::endl;

			std::wcout << wss.str();
			wof << wss.str();
		}
	}

private:
	int pid;
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
		CHAN_PTR outP3,
		CHAN_PTR outP4,
		CHAN_PTR outP5,
		CHAN_PTR outP6,
		SINK_PTR inQ1, CHAN_PTR outQ1,
		SINK_PTR inQ2, CHAN_PTR outQ2,
		MUX_PTR inC
		)
	{
		aP1 = std::make_shared<actorP>(outP1, 1);
		aP2 = std::make_shared<actorP>(outP2, 2);
		aP3 = std::make_shared<actorP>(outP3, 3);
		aP4 = std::make_shared<actorP>(outP4, 4);
		aP5 = std::make_shared<actorP>(outP5, 5);
		aP6 = std::make_shared<actorP>(outP6, 6);
		aQ1 = std::make_shared<actorQ>(inQ1, outQ1, 1);
		aQ2 = std::make_shared<actorQ>(inQ2, outQ2, 2);
		aC  = std::make_shared<actorC>(inC);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP1;		// Process P1
	std::shared_ptr<actorP> aP2;		// Process P2
	std::shared_ptr<actorP> aP3;		// Process P3
	std::shared_ptr<actorP> aP4;		// Process P4
	std::shared_ptr<actorP> aP5;		// Process P5
	std::shared_ptr<actorP> aP6;		// Process P6
	std::shared_ptr<actorQ> aQ1;		// Process Q1
	std::shared_ptr<actorQ> aQ2;		// Process Q2
	std::shared_ptr<actorC> aC;			// Process C

private:
};
//---------------------------------------------------------------------------
#endif


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

using MSG = CHAN::MSG;
//---------------------------------------------------------------------------
const int LIMIT = 10;			// Number of working cycles
//---------------------------------------------------------------------------
extern std::wofstream wof;		// Log file
//---------------------------------------------------------------------------
// Utilities
void access(std::wstring msg, std::chrono::milliseconds duration)
{
	// Simulate access to shared object
	std::wcout << msg << "\n";
	wof << msg << std::endl;
	std::this_thread::sleep_for(duration);
}
//
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
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P Class
class actorP: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{access(); Q!msg -> Q?msg}
		for(auto i = 0; i < LIMIT; i++)
		{
			std::wstringstream wss;
			wss << L"<P::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [T(acc)=" + std::to_wstring(p_in_acc(2)) + L" ms]";
			access(wss.str(), std::chrono::milliseconds(p_in_acc(2)));

			// {Q!msg}
			msg = L"P pass control to Q";
			std::wcout << msg << std::endl;
			wof << msg << std::endl;
			out->send(msg);

			// {Q?msg}
			in->recv(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q Class
class actorQ: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorQ(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{P?msg -> access(); P!msg}
		for(auto i = 0; i < LIMIT; i++)
		{
			// {P?msg}
			in->recv(msg);

			std::wstringstream wss;
			wss << L"<Q::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [T(acc)=" + std::to_wstring(p_in_acc(2)) + L" ms]";
			access(wss.str(), std::chrono::milliseconds(p_in_acc(2)));

			// {P!msg}
			msg = L"Q pass control to P";
			std::wcout << msg << std::endl;
			wof << msg << std::endl;
			out->send(msg);
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
		CHAN_PTR inP, CHAN_PTR outP,
		CHAN_PTR inQ, CHAN_PTR outQ
		)
	{
		aP = std::make_shared<actorP>(inP, outP);
		aQ = std::make_shared<actorQ>(inQ, outQ);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP;		// Process P
	std::shared_ptr<actorQ> aQ;		// Process Q

private:
};
//---------------------------------------------------------------------------
#endif


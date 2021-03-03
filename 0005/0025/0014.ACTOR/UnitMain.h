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
void exec(std::wstring msg, std::chrono::milliseconds duration)
{
	// Simulate exec to shared object
	std::wcout << msg << "\n";
	wof << msg << std::endl;
	std::this_thread::sleep_for(duration);
}
//
int p_in_exec(int pid)
{
	int dur = 0;

	switch(pid)
	{
		case 1:     // P1 in exec, ms
			dur = 500;
			break;
		case 2:     // P2 in exec, ms
			dur = 500;
			break;
		case 3:     // P3 in exec, ms
			dur = 500;
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P1 Class
class actorP1: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP1(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{exec(); P2!msg -> P3?msg}
		for(auto i = 0; i < LIMIT; i++)
		{
			std::wstringstream wss;
			wss << L"<P1::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [T(exec)=" + std::to_wstring(p_in_exec(1)) + L" ms]";
			::exec(wss.str(), std::chrono::milliseconds(p_in_exec(1)));

			// {Q!msg}
			msg = L"P1 pass control to P29";
			out->send(msg);

			// {Q?msg}
			in->recv(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process P2 Class
class actorP2: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP2(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{P1?msg -> exec(); P3!msg}
		for(auto i = 0; i < LIMIT; i++)
		{
			// {P1?msg}
			in->recv(msg);

			std::wstringstream wss;
			wss << L"<P2::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [T(exec)=" + std::to_wstring(p_in_exec(2)) + L" ms]";
			::exec(wss.str(), std::chrono::milliseconds(p_in_exec(2)));

			// {P3!msg}
			msg = L"P2 pass control to P3";
			out->send(msg);
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process P3 Class
class actorP3: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP3(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		MSG msg;

		// *{P2?msg -> exec(); P1!msg}
		for(auto i = 0; i < LIMIT; i++)
		{
			// {P2?msg}
			in->recv(msg);

			std::wstringstream wss;
			wss << L"<P3::"
				<< std::setw(3) << std::setfill(L'0')
				<< std::to_wstring(i) + L">"
				<< L" [T(exec)=" + std::to_wstring(p_in_exec(3)) + L" ms]";
			::exec(wss.str(), std::chrono::milliseconds(p_in_exec(3)));

			// {P1!msg}
			msg = L"P3 pass control to P1";
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
		CHAN_PTR inP1, CHAN_PTR outP1,
		CHAN_PTR inP2, CHAN_PTR outP2,
		CHAN_PTR inP3, CHAN_PTR outP3
		)
	{
		aP1 = std::make_shared<actorP1>(inP1, outP1);
		aP2 = std::make_shared<actorP2>(inP2, outP2);
		aP3 = std::make_shared<actorP3>(inP3, outP3);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP1> aP1;		// Process P1
	std::shared_ptr<actorP2> aP2;		// Process P2
	std::shared_ptr<actorP3> aP3;		// Process P3

private:
};
//---------------------------------------------------------------------------
#endif


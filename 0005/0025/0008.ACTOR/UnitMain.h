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
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Process P1 Class
//  Producer 1
class actorP1: public proactor<CHAN_PTR>
{
public:
	actorP1(CHAN_PTR out) {start(out);}

protected:
	void body(CHAN_PTR out)	override
	{
		for(auto i = 0; i < 3*LIMIT; i++)
		{
			// {Q1 ! msg}
			std::wstringstream ws;
			ws << L"<P1::"
			   << std::setw(2) << std::setfill(L'0')
			   << std::to_wstring(i) << L">";

			MSG msg = ws.str();
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Px Class
//  Producer x
//  P2 = P3 = P4 = Px
class actorPx: public proactor<CHAN_PTR, int>
{
public:
	actorPx(CHAN_PTR out, int pid) {start(out, pid);}

protected:
	void body(CHAN_PTR out, int pid)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			// {Q2 ! msg}
			std::wstringstream wss;
			wss << L"<P" << std::to_wstring(pid + 1) << L"::"
				<< std::setw(2) << std::setfill(L'0')
				<< std::to_wstring(i) << L">";

			MSG msg = wss.str();
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q1 Class
//  Buffer 1
class actorQ1: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorQ1(CHAN_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		for(auto i = 0; i < 3*LIMIT; i++)
		{
			// {P1 ? dst}
			MSG dst;
			in->recv(dst);

			// {C ! msg}
			std::wstringstream wss;
			wss << dst << L" <Q1::"
				<< std::setw(2) << std::setfill(L'0')
				<< std::to_wstring(i) << L">";

			MSG msg = wss.str();
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process Q2 Class
//  Buffer 2
class actorQ2: public proactor<MUX_PTR, CHAN_PTR>
{
public:
	actorQ2(MUX_PTR in, CHAN_PTR out) {start(in, out);}

protected:
	void body(MUX_PTR in, CHAN_PTR out)	override
	{
		for(auto i = 0; i < 3*LIMIT; i++)
		{
			// {P2 ? dst [] P3 ? dst [] P4 ? dst}
			MSG dst;
			int id = in->recv(dst);

			// {C ! msg}
			std::wstringstream wss;
			wss << dst << L" <Q2::"
				<< std::setw(2) << std::setfill(L'0')
				<< std::to_wstring(i) << L"::" << std::to_wstring(id + 1) << L">";

			MSG msg = wss.str();
			out->send(msg);

			// Delay to check for Race Conditions
//			std::this_thread::sleep_for(std::chrono::milliseconds(2));
//			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

private:
	int pid;
};
//---------------------------------------------------------------------------
//  Process C Class
//  Consumer
class actorC: public proactor<MUX_PTR>
{
public:
	actorC(MUX_PTR in) {start(in);}

protected:
	void body(MUX_PTR in)	override
	{
		try
		{
			auto tp = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(tp);

			std::wstringstream fn;
			fn << std::put_time(std::localtime(&tt), L"..\\..\\Logs\\Log.%Y%m%d.%H%M%S.txt");
			std::wofstream wof(fn.str());

			for(auto i = 0; i < (3 + 3)*LIMIT; i++)
			{
				// {Q1 ? dst [] Q2 ? dst}
				MSG dst;
				int id = in->recv(dst);

				std::wstringstream wss;
				wss << "#" << std::setw(2) << std::setfill(L'0') << std::to_wstring(i) << " "
					<< dst << L" <C::"
					<< std::setw(2) << std::setfill(L'0')
					<< std::to_wstring(i) << L"::" << std::to_wstring(id + 1) + L">"
					<< std::endl;
				std::wstring ws = wss.str();

				std::wcout << ws;
				wof << ws;
			}

			std::wcout << L"The Log saved to <" << fn.str() << L">" << std::endl;
		}
		catch(std::exception& e)
		{
			e.what();
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
		CHAN_PTR inQ1, CHAN_PTR outQ1,
		MUX_PTR inQ2, CHAN_PTR outQ2,
		MUX_PTR inC)
	{
		aP1 = std::make_shared<actorP1>(outP1);
		aP2 = std::make_shared<actorPx>(outP2, 2);
		aP3 = std::make_shared<actorPx>(outP3, 3);
		aP4 = std::make_shared<actorPx>(outP4, 4);
		aQ1 = std::make_shared<actorQ1>(inQ1, outQ1);
		aQ2 = std::make_shared<actorQ2>(inQ2, outQ2);
		aC  = std::make_shared<actorC>(inC);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//  No any AO controller/dispatcher
		//  All processes will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP1> aP1;	// Process P1
	std::shared_ptr<actorPx> aP2;	// Process P2
	std::shared_ptr<actorPx> aP3;	// Process P3
	std::shared_ptr<actorPx> aP4;	// Process P4
	std::shared_ptr<actorQ1> aQ1;	// Process Q1
	std::shared_ptr<actorQ2> aQ2;	// Process Q2
	std::shared_ptr<actorC>  aC;	// Process C

private:
};
//---------------------------------------------------------------------------
#endif


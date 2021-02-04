/////////////////////////////////////////////////////////////////////////////
//	Project Name
//    CSP_3P_CoP.06RC => CSP_3P_CoP.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Coprocedures (CoP)
//		Project 0005.0014 update
//		S = {P1 || P2 || P3}
//		0014.1.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.5.1. CoP.png
//		0014.2.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.5.2. CoP.png
//		Logs
//
//		csp namespace ver. 0.91RC >>>
//	Status: Completed
/////////////////////////////////////////////////////////////////////////////
#pragma hdrstop
#pragma argsused

#ifdef _WIN32
#include <tchar.h>
#else
  typedef char _TCHAR;
  #define _tmain main
#endif

#include "UnitMain.h"
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
std::wofstream wof;         // Log file
//------------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	auto tp = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(tp);

	std::wstringstream fn;
	// https://en.cppreference.com/w/cpp/io/manip/put_time
	fn << std::put_time(std::localtime(&tt), L"..\\..\\Logs\\Log.%Y%m%d.%H%M%S.txt");
	wof.open(fn.str());

	////////////////////////////////////////////////////////////////////////////
	//  COMMUNICAIONS MEDIUM
	////////////////////////////////////////////////////////////////////////////
	//
	//  Channels
	auto chanP1P2 = std::make_shared<CHAN>(L"ChanP1P2");
	auto chanP2P3 = std::make_shared<CHAN>(L"ChanP2P3");
	auto chanP3P1 = std::make_shared<CHAN>(L"ChanP3P1");
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Coprocedures (CoP)" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	S = {P1 || P2 || P3}
	//
	vThreads.push_back(std::thread(doP1, chanP3P1, chanP1P2));
	vThreads.push_back(std::thread(doP2, chanP1P2, chanP2P3));
	vThreads.push_back(std::thread(doP3, chanP2P3, chanP3P1));
	////////////////////////////////////////////////////////////////////////////

	for(auto& t: vThreads)
	{
		if(t.joinable())
		{
			t.join();
		}
	}

	if(wof.good())
	{
		std::wcout << L"The Log saved to <" << fn.str() << L">" << std::endl;
	}
	wof.close();
	std::cout << ">>> End <<<" << std::endl;

	// "Press any key to continue..."
	std::cout << std::endl << std::endl;
	system("pause");

	return 0;
}
//------------------------------------------------------------------------------
//  Main function of process P1
void doP1(CHAN_PTR in, CHAN_PTR out)
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
		exec(wss.str(), std::chrono::milliseconds(p_in_exec(1)));

		// {Q!msg}
		msg = L"P1 pass control to P29";
		out->send(msg);

		// {Q?msg}
		in->recv(msg);
	}
}
//------------------------------------------------------------------------------
//  Main function of process P2
void doP2(CHAN_PTR in, CHAN_PTR out)
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
		exec(wss.str(), std::chrono::milliseconds(p_in_exec(2)));

		// {P3!msg}
		msg = L"P2 pass control to P3";
		out->send(msg);
	}
}
//------------------------------------------------------------------------------
//  Main function of process P3
void doP3(CHAN_PTR in, CHAN_PTR out)
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
		exec(wss.str(), std::chrono::milliseconds(p_in_exec(3)));

		// {P1!msg}
		msg = L"P3 pass control to P1";
		out->send(msg);
	}
}
//------------------------------------------------------------------------------
void exec(std::wstring msg, std::chrono::milliseconds duration)
{
	// Simulate exec to shared object
	std::wcout << msg << "\n";
	wof << msg << std::endl;
	std::this_thread::sleep_for(duration);
}
//------------------------------------------------------------------------------


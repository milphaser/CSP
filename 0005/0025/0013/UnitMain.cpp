/////////////////////////////////////////////////////////////////////////////
//	Project Name
//	  CSP_1P1Q_AASHO.06RC => CSP_1P1Q_AASHO.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Alternate access to shared object (AASHO)
//		As a protected shared object is used std::wcout
//		Project could be considered as example of 2 coprocedures system.
//		Project 0005.0013 update
//		S = {P || Q}
//		0013.1.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.4.1. AASHO.png
//		0013.2.png C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.4.2. AASHO.png
//		Logs
//
//		csp namespace ver. 0.91RC >>>
//	Status:	Completed
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
	auto chanPQ = std::make_shared<CHAN>(L"ChanPQ");
	auto chanQP = std::make_shared<CHAN>(L"ChanQP");
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Alternate access to shared object" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> vThreads;
	//
	//	S = {P || Q}
	//
	vThreads.push_back(std::thread(doP, chanQP, chanPQ));
	vThreads.push_back(std::thread(doQ, chanPQ, chanQP));
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
//  Main function of process P
void doP(CHAN_PTR in, CHAN_PTR out)
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
//------------------------------------------------------------------------------
//  Main function of process Q
void doQ(CHAN_PTR in, CHAN_PTR out)
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
//------------------------------------------------------------------------------
void access(std::wstring msg, std::chrono::milliseconds duration)
{
	// Simulate access to shared object
	std::wcout << msg << "\n";
	wof << msg << std::endl;
	std::this_thread::sleep_for(duration);
}
//------------------------------------------------------------------------------


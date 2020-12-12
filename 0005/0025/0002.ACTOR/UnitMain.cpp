/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_1P1Q.091RC => CSP_ACTOR_1P1Q.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Producer-Customer Type 1:1
//		{P || Q}
//		0017.0002.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 2.1. Pair of communicating processes.png
//
//		There we have next scenarious:
//      1. Delay(P) > Delay(Q) => O'K
//		2. Delay(P) < Delay(Q) => The need of buffering between P and Q, if not used CSP channel
//		3. Delay(P) = Delay(Q) = 0 => The same as above conclusion
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
	auto chanPQ = std::make_shared<CHAN>(L"ChanPQ", SYNC);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Producer-Customer Type 1:1" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//	S = {P || Q}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	{
		parsys parSys
		(
			chanPQ,		// actorP
			chanPQ		// actorQ
		);
	}   // 	RAII => join
	//
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	//  Run Parameters/Report
	//
	{
		std::wstringstream wss;
		wss << "+++" << std::endl
			<< L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl
			<< "Scenario #" << SCENARIO << std::endl;
		if(SYNC == CHAN::Sync::unilateral)
		{
			wss << "Unilateral (not CSP) synchronization" << std::endl;
		}
		else if(SYNC == CHAN::Sync::bilateral)
		{
			wss << "Bilateral (default) synchronization" << std::endl;
		}

		std::wcout << wss.str();
		wof << wss.str();
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


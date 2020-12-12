/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_2P1Q.04 => CSP_2P1Q.07RC => CSP_2P1Q.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Producer-Customer Type 2:1 (Non-determinism)
//		{P1 || Q || P2}
//		0017.0007.1.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 4. 2-channel non-deterministic selection.png
//		0017.0007.2.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.1. mux (n to 1).png
//
//		csp namespace ver. 0.7RC >>>
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
	auto chanP1Q = std::make_shared<CHAN>(L"ChanP1Q");
	auto chanP2Q = std::make_shared<CHAN>(L"ChanP2Q");
	//
	// Process Q input channels multiplex setup
	MUX_PTR muxInQ = std::make_shared<MUX>();
	muxInQ->add(chanP1Q);
	muxInQ->add(chanP2Q);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Producer-Customer Type 2:1 (Non-determinism)" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//	S = {P1 || Q || P2}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	{
		parsys parSys
		(
			   chanP1Q,		// actorP1
			   chanP2Q,		// actorP2
			   muxInQ   	// actorQ (AO Controller/Dispatcher)
		);
	}   // 	RAII => join
	//
	////////////////////////////////////////////////////////////////////////////

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


/////////////////////////////////////////////////////////////////////////////
//	Project Name
//      CSP_3P1Q_sink.08RC => CSP_3P1Q_sink.091RC => CSP_ACTOR_3P1Q_sink.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Sink type input (n:1/n) Testbed
//		Project 005.0018 update
//		S = {P1 || P2 || P3 || Q}
//		Q = {{P1 ? dst[1] || P2 ? dst[2] ||P3 ? dst[3]}; print(dst);}
//
//		0018.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.1.2. sink (n to 1).png
//      Logs
//
//		[csp namespac ver. 0.91RC]
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
	auto chanP3Q = std::make_shared<CHAN>(L"ChanP3Q");
	//
	// Process Q input channels sink setup
	SINK_PTR sinkInQ = std::make_shared<SINK>();
	sinkInQ->add(chanP1Q);
	sinkInQ->add(chanP2Q);
	sinkInQ->add(chanP3Q);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Sink type input (n:1/n) Testbed" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;

	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//  S = {P1 || P2 || P3 || Q}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	{
		parsys parSys(
			   chanP1Q,		// actorP1
			   chanP2Q,		// actorP2
			   chanP3Q,		// actorP3
			   sinkInQ   	// actorQ
			   );   	// actorQ
	}   // 	RAII => join
	//
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	//  Run Parameters/Report
	//
	{
		std::wstringstream wss;
		wss << L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl;
		if(sinking == SINK::Sinking::by_place)
		{
			wss << L"Sinking: by_place" << std::endl;
		}
		else if(sinking == SINK::Sinking::parallel)
		{
			wss << L"Sinking: parallel (default)" << std::endl;
		}

		std::wcout << wss.str();
		wof << wss.str();
	}
	//
	for(auto i = 0; i < 3; i++)
	{
		std::wstringstream wss;
		wss << L"RCC Delay P" << std::to_wstring(i+1)
			<< L": " << std::setw(4) << std::to_wstring(rcc_delay(i+1))
			<< L" ms" << std::endl;

		std::wcout << wss.str();
		wof << wss.str();
	}
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


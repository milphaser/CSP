/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_3P_CoP.06RC => CSP_3P_CoP.091RC => CSP_ACTOR_3P_CoP.091RC
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
	//
	//	S = {P1 || P2 || P3}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	{
		parsys parSys(
			   chanP3P1, chanP1P2,		// actorP1
			   chanP1P2, chanP2P3,		// actorP2
			   chanP2P3, chanP3P1		// actorP3
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


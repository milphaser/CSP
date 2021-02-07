/////////////////////////////////////////////////////////////////////////////
//	Project Name
//      CSP_CCS.04 => CSP_CCS.091RC => CSP_ACTOR_CCS.091RC
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Complex Communication Scheme (CCS) Testbed
//		Project 0005.0008 update
// 		S = {P1 || P2 || P3 || P4 || Q1 || Q2 || C}
//      0008.png
//      C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. 8.D. Exemplary sample system.png
//
//		csp namespac ver. 0.91RC
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
int _tmain(int argc, _TCHAR* argv[])
{
	////////////////////////////////////////////////////////////////////////////
	//  COMMUNICAIONS MEDIUM
	////////////////////////////////////////////////////////////////////////////
	//
	//  Channels
	auto chanP1Q1 = std::make_shared<CHAN>(L"ChanP1Q1");
	//
	auto chanP2Q2 = std::make_shared<CHAN>(L"ChanP2Q2");
	auto chanP3Q2 = std::make_shared<CHAN>(L"ChanP3Q2");
	auto chanP4Q2 = std::make_shared<CHAN>(L"ChanP4Q2");
	//
	auto chanQ1C = std::make_shared<CHAN>(L"ChanQ1C");
	auto chanQ2C = std::make_shared<CHAN>(L"ChanQ2C");
	//
	// Process Q2 input channels multiplex
	MUX_PTR muxInQ2 = std::make_shared<MUX>();
	muxInQ2->add(chanP2Q2);
	muxInQ2->add(chanP3Q2);
	muxInQ2->add(chanP4Q2);
	//
	// Process C input channels multiplex
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanQ1C);
	muxInC->add(chanQ2C);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Complex Communication Scheme (CCS) Testbed" << std::endl;
	std::cout << std::endl << ">>> Test Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//  S = {P1 || P2 || P3 || P4 || Q1 || Q2 || C}
	//
	{
		parsys parSys
		(
			chanP1Q1,			// actorP1
			chanP2Q2,			// actorPx
			chanP3Q2,			// actorPx
			chanP4Q2,			// actorPx
			chanP1Q1, chanQ1C,	// actorQ1
			muxInQ2, chanQ2C,	// actorQ2
			muxInC				// actorC
		);
	}   // 	RAII => join
	//
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Test End >>>" << std::endl;

	// "Press any key to continue..."
	std::cout << std::endl << std::endl;
	system("pause");

	return 0;
}
//------------------------------------------------------------------------------


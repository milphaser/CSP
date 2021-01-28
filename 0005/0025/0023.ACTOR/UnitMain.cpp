/////////////////////////////////////////////////////////////////////////////
//	Project Name
//		CSP_ACTOR_MB.091RC
//		[CSP_ACTOR_MB.09RC]
/////////////////////////////////////////////////////////////////////////////
//	Description
//		Application: Actor Implementation of Mailbox/Active Mailbox
//		Identical to Project 0005.0023 [CSP_ACTOR_MB.091RC]
//		S = {P || C || Q}
//
//		0021.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.7. ASO.png
//		C:\My_CONFERENCES\__CONFERENCES__\RU\аг2020\FIG\Fig. x.7. Spec.ASO.CentralServer.docx
//
//		Actors Modes:
//		1 - Actors as separate local objects
//		2 - Actors as separate dynamic objects
//		3 - Actors encapsulated in one object (*)
//
//		DISTINGTION:
//		AO Controller (C) should be explicitly joined
//		before destruction as it containts the shared object
//		(in this case it is the queue).
//
//		[csp namespace ver. 0.91RC]
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
#include <sstream>
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
	auto chanPC = std::make_shared<CHAN>(L"ChanPC", SYNC);
	auto chanCP = std::make_shared<CHAN>(L"ChanCP", SYNC);
	//
	auto chanQC = std::make_shared<CHAN>(L"ChanQC", SYNC);
	auto chanCQ = std::make_shared<CHAN>(L"ChanCQ", SYNC);
	//
	// Process C input channels multiplex setup
	MUX_PTR muxInC = std::make_shared<MUX>();
	muxInC->add(chanPC);
	muxInC->add(chanQC);
	//
	// Process C output channels fork setup
	FORK_PTR forkOutC = std::make_shared<FORK>();
	forkOutC->add(chanCP);
	forkOutC->add(chanCQ);
	////////////////////////////////////////////////////////////////////////////

	std::cout << ">>> Application: Active Shared Object/Active Mailbox" << std::endl;
	std::cout << "<<< Run >>>" << std::endl;
	////////////////////////////////////////////////////////////////////////////
	//  PROCESSES
	////////////////////////////////////////////////////////////////////////////
	//
	//	S = {P || C || Q}
	//
	////////////////////////////////////////////////////////////////////////////
	//
	if(ACTORS_MODE == 1)
	{	//  I. Actors as separate local objects
		actorP aP(std::ref(wof), chanCP, chanPC);
		actorQ aQ(std::ref(wof), chanCQ, chanQC);
		actorC aC(std::ref(wof), muxInC, forkOutC);

		//	AO Controller (C) should be explicitly joined
		// 	before destruction as it containts the shared object
		//	(in this case this is the queue).
		aC.join();
	}   //  RAII => P and Q will be joined implicitly at this point in their destructor
	else if(ACTORS_MODE == 2)
	{	//  II. Actors as separate dynamic objects
		std::shared_ptr<actorP> aP = std::make_shared<actorP>(std::ref(wof), chanCP, chanPC);
		std::shared_ptr<actorQ> aQ = std::make_shared<actorQ>(std::ref(wof), chanCQ, chanQC);
		std::shared_ptr<actorC> aC = std::make_shared<actorC>(std::ref(wof), muxInC, forkOutC);
		//
		//	AO Controller (C) should be explicitly joined
		// 	before destruction as it containts the shared object
		//	(in this case it is the queue).
		aC->join();
	}   //  RAII => P and Q will be joined implicitly at this point in their destructor
		//  or later in their destructor if are defined outside of this brackets
	else
	{	//  III. Actors encapsulated in one object of type <parsys>
		parsys parSys(std::ref(wof),		// common log file reference
				   chanCP, chanPC,      	// actorP
				   chanCQ, chanQC,      	// actorQ
				   muxInC, forkOutC);   	// actorC (AO controller)
	}
	//
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	//  Run Parameters/Report
	//
	{
		std::wstringstream wss;
		wss << "+++" << std::endl
			<< L"Working Cycles: " + std::to_wstring(LIMIT) << std::endl
			<< "Actors Mode #" << ACTORS_MODE << std::endl
			<< "Scenario #" << SCENARIO << std::endl
			<< "Max buf size #" << MAX_BUFFER_SIZE << std::endl;
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


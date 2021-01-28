//---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH

#include "csp.h"
#include "proactor.h"
//---------------------------------------------------------------------------
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <thread>
//---------------------------------------------------------------------------
using __MSG = std::pair<std::wstring,		// command
						  std::wstring>;    // data

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
//  Actors Mode
const int ACTORS_MODE   = 3;    // 1 - Actors as separate local objects
								// 2 - Actors as separate dynamic objects
								// 3 - Actors encapsulated in one object
//---------------------------------------------------------------------------
//	RCC delay scenario
const int SCENARIO 		= 3;	// 1 - Delay(P) >> Delay(Q)
								// 2 - Delay(P) << Delay(Q)
								// 3 - Delay(P) = Delay(Q) = 0
//---------------------------------------------------------------------------
//const CHAN::Sync SYNC = CHAN::Sync::unilateral;
const CHAN::Sync SYNC = CHAN::Sync::bilateral;
//---------------------------------------------------------------------------
const std::wstring MSG_PUT	= L"put";	// put message type
const std::wstring MSG_ACK	= L"ack";	// ack message type
const std::wstring MSG_GET	= L"get";	// get message type
const std::wstring MSG_RPL	= L"rpl";	// rpl message type

const int MAX_BUFFER_SIZE 	= 1;		// message buffer max size
//---------------------------------------------------------------------------
//	Utilities
//  Race Conditions Check Delay
int rcc_delay(int pid)
{
	int dur = 0;

	switch(pid)
	{
		case 1:     // P delay, ms
			if(SCENARIO == 1)
			{
				dur = 1000;
			}
			else if(SCENARIO == 2)
			{
				dur = 100;
			}
			break;
		case 2:     // Q delay, ms
			if(SCENARIO == 1)
			{
				dur = 100;
			}
			else if(SCENARIO == 2)
			{
				dur = 1000;
			}
			break;
	}

	return dur;
}
//---------------------------------------------------------------------------
//  Instantiations of proactor classes
//---------------------------------------------------------------------------
//  Producer Class
class actorP: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorP(std::wofstream& fn, CHAN_PTR in, CHAN_PTR out): wof(fn) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			//  Generate new message
			MSG msg;
			msg.second = std::to_wstring(i);

			//  {C!<put, data>}
			msg.first = MSG_PUT;
			out->send(msg);	// send <put, data>

			//	{C?<ack>}
			in->recv(msg);      	// recv <ack>

			//  Race Condition Check Delay
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(1)));
		}
	}

private:
	std::wofstream& wof;
};
//---------------------------------------------------------------------------
//  Consumer Class
class actorQ: public proactor<CHAN_PTR, CHAN_PTR>
{
public:
	actorQ(std::wofstream& fn, CHAN_PTR in, CHAN_PTR out): wof(fn) {start(in, out);}

protected:
	void body(CHAN_PTR in, CHAN_PTR out)	override
	{
		for(auto i = 0; i < LIMIT; i++)
		{
			MSG msg;

			//	{C!<get>}
			msg.first = MSG_GET;
			out->send(msg);			// send <get>

			//	{C?<rpl, data>}
			in->recv(msg);      	// recv <rpl, data>

			std::wcout << msg.second << std::endl;
			wof << msg.second << std::endl;

			//  Race Condition Check Delay
			std::this_thread::sleep_for(std::chrono::milliseconds(rcc_delay(2)));
		}
	}

private:
	std::wofstream& wof;
};
//---------------------------------------------------------------------------
//  AO Controller Class
class actorC: public proactor<MUX_PTR, FORK_PTR>
{
public:
	const int MAX_BUFFER_SIZE 	= 1;		// message buffer max size

	actorC(std::wofstream& fn, MUX_PTR in, FORK_PTR out): wof(fn) {start(in, out);}

protected:
	void body(MUX_PTR in, FORK_PTR out)	override
	{
		for(auto i = 0; i < 2*LIMIT; i++)
		{
			MSG msg;

			//	{P?msg [] Q?msg}
			auto pid = in->recv(msg);    // recv <req> non-deterministically

			if(msg.first == MSG_PUT)
			{
				OnReceiptOfPut(out, msg);
			}
			else if(msg.first == MSG_GET)
			{
				OnReceiptOfGet(out);
			}
		}
	}

	std::queue <MSG> qBuffer;	// message buffer
	bool boolReqPut;            // pending put request
	bool boolReqGet;            // pending get request

	//  Event Handlers ---
	void OnInit(void)	override
	{
		qBuffer = {};			// clear message buffer

		boolReqPut = false;
		boolReqGet = false;
	}
	void OnReceiptOfPut(FORK_PTR out, MSG msg)
	{
		MSG rpl;

		qBuffer.push(msg);

		if(boolReqGet)
		{
			//  <get> released
			boolReqGet = false;

			rpl = qBuffer.front();
			qBuffer.pop();

			//  {Q!<rpl, data>}
			rpl.first = MSG_RPL;
			out->send(rpl, 1);		// send <rpl, data> to Q
		}

		if(qBuffer.size() < MAX_BUFFER_SIZE)
		{
			//  {P!<ack>}
			rpl.first = MSG_ACK;
			out->send(rpl, 0);			// send <ack> to P
		}
		else
		{
			//	<put> held
			boolReqPut = true;
			std::wstring ws = L"<put> held";
			std::wcout << ws << std::endl;
			wof << ws << std::endl;
		}
	}
	void OnReceiptOfGet(FORK_PTR out)
	{
		MSG rpl;

		if(!qBuffer.empty())
		{
			rpl = qBuffer.front();
			qBuffer.pop();

			//  {Q!<rpl, data>}
			rpl.first = MSG_RPL;
			out->send(rpl, 1);	 		// send <rpl, data> to Q

			if(boolReqPut)
			{
				//  <put> released
				boolReqPut = false;

				//  {P!<ack>}
				rpl.first = MSG_ACK;
				out->send(rpl, 0);		// send <ack> to P
			}
		}
		else
		{
			//  <get> held
			boolReqGet = true;
			std::wstring ws = L"<get> held";
			std::wcout << ws << std::endl;
			wof << ws << std::endl;
		}
	}

private:
	std::wofstream& wof;
};
//---------------------------------------------------------------------------
//  Class Parallel System
//  encapsulates all system actors
class parsys
{
public:
	parsys(std::wofstream& wof,
		CHAN_PTR inP, CHAN_PTR outP,
		CHAN_PTR inQ, CHAN_PTR outQ,
		MUX_PTR inC, FORK_PTR outC)
	{
		aP = std::make_shared<actorP>(wof, inP, outP);
		aQ = std::make_shared<actorQ>(wof, inQ, outQ);
		aC = std::make_shared<actorC>(wof, inC, outC);
	}
	~parsys(void)  {join();}
	void join(void)
	{
		//	AO Controller (C) should be explicitly joined
		// 	before destruction as it containts the shared object
		//	(in this case it is the queue).
		aC->join();
		//  P and Q will be joined implicitly later in their destructor
	}

protected:
	std::shared_ptr<actorP> aP;     // Producer
	std::shared_ptr<actorQ> aQ;     // Consumer
	std::shared_ptr<actorC> aC;     // AO Controller

private:
};
//---------------------------------------------------------------------------
#endif


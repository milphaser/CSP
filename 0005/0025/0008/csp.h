/////////////////////////////////////////////////////////////////////////////
//	csp namespac ver. 0.91RC
//  [identical to ver. 0.9RC but refactored: public members before]
//
// Copyright (c) 2017-2020 by Milen Loukantchevsky.
/////////////////////////////////////////////////////////////////////////////
//
//	csp::chan
//		csp::chan::recv()
//		csp::chan::send(csp::chan::Sync)
//		- bilateral  (default) synchronization
//		- unilateral (not CSP) synchronization
//
//	csp::mux
//		мултиплексиране по вход (n:1 communication by input)
//	csp::mux::recv()
//		csp::alt() replacement
//		n-channels, non-deterministic/two dispatching modes
//		- by place
//		- non-deterministic (default)
//	csp::mux::recv(cid)
//      unicast receive
//
//	csp::sink
//		вилка по вход (n:1/n communication by input)
//	csp::sink::recv()
//		n-channels/two sinking modes
//		- by place
//		- parallel (default)
//
//	csp::fork
//		вилка по изход (1:n communication by output/broadcast)
//	csp::fork::send()
//		n-channels/two broadcast modes
//		- by place
//		- parallel broadcast (default)
//	csp::fork::send(cid)
//		unicast send mode
//
//	Status: Completed
/////////////////////////////////////////////////////////////////////////////

#ifndef cspH
#define cspH
//---------------------------------------------------------------------------
#include <condition_variable>
#include <exception>
#include <mutex>
#include <random>
#include <thread>
#include <vector>
//---------------------------------------------------------------------------
namespace csp
{
	//  Non-deterministic Uniform Dispatching Random Engine Setup
	static auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	static std::default_random_engine random_engine(seed);  // randomization with seed

	template <class T_ID, class T_DATA> class chan;
	template <class T_ID, class T_DATA> class mux;
	template <class T_ID, class T_DATA> class sink;
	template <class T_ID, class T_DATA> class fork;

	/////////////////////////////////////////////////////////////////////////
	//  CSP Channel Template
	//
	template <class T_ID, class T_DATA>
	class chan
	{
	public:
		// Synchronization Modes
		enum class Sync {bilateral, unilateral};
		//  bilateral  - rendezvous (default) synchronization
		//  unilateral - alternative not CSP synchronization

		using MSG = T_DATA;             // typedef T_DATA msg

		explicit chan(T_ID chan_id, Sync s = Sync::bilateral): id{chan_id}
		{
			id = chan_id;
			sent = received = false;
			sync = s;

			mux = nullptr;
		}

		T_ID get_id(void) const { return id;}
		bool get_sent_status(void) const {return sent;}
		void tie(csp::mux<T_ID, T_DATA>* const mux_in) {mux = mux_in;}
		Sync get_sync(void) {return sync;}

		// Primitive ?
		void recv(T_DATA& dst)
		{
			{
				std::unique_lock<std::mutex> lk(mu_cv); 	// RAII
				cv.wait(lk, [this]()->bool{return this->sent;});
				sent = false;
				dst = transient;		// copy received message to the destination
				received = true;
				// Automatic unlocking of lk before notifying because of RAII
			}
			// Return back acknowledge to the sender
			cv.notify_one();
		}

		// Primitive !
		// Send message <src> to Input Channels Multiplexer
		void send(T_DATA& src)
		{
			{
				std::lock_guard<std::mutex> lk(mu_cv);      // RAII
				transient = src;		// copy message to transient buffer
				sent = true;
				// Automatic unlocking of lk before notifying because of RAII
			}
			cv.notify_one();
			//
			// Input Channels Multiplexer Signaling
			if(mux != nullptr) mux->notify();
			//
			if(sync == Sync::bilateral)
			{   // Wait for acknowledge from the receiver
				std::unique_lock<std::mutex> lk(mu_cv);     // RAII
				cv.wait(lk, [this]()->bool{return this->received;});
				received = false;
				// Automatic unlocking of lk on exit because of RAII
			}
		}
	protected:
		T_ID id;                    	// channel identifier
		T_DATA transient;          		// message in transfer channel part

		std::mutex mu_cv;            	// mutex of the conditional variable
		std::condition_variable cv;     // conditional variable to synchronize on
		bool sent;			// sent flag (Set by Sender/Reset by Receiver)
		bool received;      // received flag (Set by Receiver/Reset by Sender)
		Sync sync;          			// synchronization mode

		csp::mux<T_ID, T_DATA>* mux;    // link to the input channels multiplexer
	};
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	//  Input Channels Multiplex Template
	//  мултиплексиране по вход (n:1 communication by input)
	template <class T_ID, class T_DATA>
	class mux
	{
	public:
		using CHAN = csp::chan<T_ID, T_DATA>;

		// Dispatching Modes
		enum class Dispatching {by_place, uniform};
		//  by_place  - deterministic mode used for illustrative purposes
		//  uniform   - default (non-deterministic) mode

		void add(const std::shared_ptr<CHAN> ch)
		{
			ch->tie(this);
			in.push_back(ch);
		}

		std::shared_ptr<CHAN>& operator[](const int cid) const
		{
			if((0 <= cid) && (cid < in.size()))
			{
				return in[cid];
			}

			return nullptr;
		}
		void notify(void) {cv.notify_one();}

		//  Alternative Command n-channel
		//  Dispatching:
		//		uniform  - default (non-deterministic) mode
		//  	by_place - deterministic mode used for illustrative purposes
		int recv(T_DATA& dst, Dispatching dispatch = Dispatching::uniform)
		{
			int id = -1;

			//  WaitOnForMultipleEvents
			std::unique_lock<std::mutex> lk(mu_cv);
			cv.wait(lk,
					[this, &id, &dispatch]()
					{	//  The Predicate, implemented as lambda
						//  checked before std::wait(lock)
						if(dispatch == Dispatching::by_place)
						{
							// Dispatching by guard place
							for(auto j = 0; j < this->in.size(); j++)
							{
								if(this->in[j]->get_sent_status() == true)
								{
									id = j;
									return true;
								}
							}
						}
						else if(dispatch == Dispatching::uniform)
						{
							// Non-deterministic uniform dispatching
							std::vector<int> vGuardsInTrue;
							for(auto j = 0; j < this->in.size(); j++)
							{
								if(this->in[j]->get_sent_status() == true)
								{
									vGuardsInTrue.push_back(j);
								}
							}

							if(vGuardsInTrue.size() > 0)
							{
								int k = 0;  // index of selected true guard
								if(vGuardsInTrue.size() > 1)
								{   // more than 1 guards are true
									// select one of them randomly
									std::uniform_int_distribution<int> distribution(0, vGuardsInTrue.size() - 1);
									k = distribution(random_engine);
								}

								id = vGuardsInTrue[k];
								return true;
							}
						}

						id = -1;
						return false;
					});

			in[id]->recv(dst);

			return id;
		}

		//	Unicast receive
		void recv(T_DATA& dst, const int cid)
		{
			if((0 <= cid) && (cid < in.size()))
			{
				in[cid]->recv(dst);
			}
		}

	protected:
		std::mutex mu_cv;            	// mutex of the conditional variable
		std::condition_variable cv;     // conditional variable to synchronize on
		std::vector<std::shared_ptr<CHAN>> in;
	};
	/////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	//  Input Channels Sink Template
	//  вилка по вход (n:1/n communication by input)
	template <class T_ID, class T_DATA>
	class sink
	{
	public:
		using CHAN = csp::chan<T_ID, T_DATA>;

		// Input Sink Modes
		enum class Sinking {by_place, parallel};
		// 	by_place - sequential mode of sinking
		//	parallel - default sinking

		void add(const std::shared_ptr<CHAN> ch)
		{
			in.push_back(ch);
		}

		std::shared_ptr<CHAN>& operator[](const int cid) const
		{
			if((0 <= cid) && (cid < in.size()))
			{
				return in[cid];
			}

			return nullptr;
		}

		//  Sinking Command n-channel
		//  Dispatching:
		//  	by_place - sequential mode of sinking
		//		parallel - default sinking
		std::vector<T_DATA> recv(Sinking sinking = Sinking::parallel)
		{
			std::vector<T_DATA> dst;
			for(auto cid = 0; cid < in.size(); ++cid)
			{
				T_DATA d;
				dst.push_back(d);
			}

			if(sinking == Sinking::by_place)
			{
				for(auto cid = 0; cid < in.size(); cid++)
				{
				   in[cid]->recv(dst[cid]);
				}
			}
			else if(sinking == Sinking::parallel)
			{
				std::vector<std::thread> vt;

				for(auto cid = 0; cid < in.size(); ++cid)
				{
					vt.push_back(std::thread([this, &cid, &dst]()
											{
												this->in[cid]->recv(dst[cid]);
											}));
				}

				for(auto& t: vt)
				{
					if(t.joinable())
					{
						t.join();
					}
				}
			}

			return dst;
		}

	protected:
		std::vector<std::shared_ptr<CHAN>> in;
	};

	/////////////////////////////////////////////////////////////////////////
	//  Output Channels Fork Template
	//  вилка по изход (1:n communication by output/broadcast)
	template <class T_ID, class T_DATA>
	class fork
	{
	public:
		using CHAN = csp::chan<T_ID, T_DATA>;

		// Output Fork Broadcasting Modes
		enum class Broadcasting {by_place, parallel};
		//  by_place  - sequential broadcast mode
		//  parallel  - default broadcast mode

		void add(const std::shared_ptr<CHAN> ch)
		{
			out.push_back(ch);
		}

		std::shared_ptr<CHAN>& operator[](const int cid) const
		{
			if((0 <= cid) && (cid < out.size()))
			{
				return out[cid];
			}

			return nullptr;
		}

		//  Broadcast Command n-channel
		//  Dispatching:
		//  by_place  - sequential broadcast mode
		//  parallel  - default broadcast mode
		void send(T_DATA& src, Broadcasting broadcast = Broadcasting::parallel)
		{
			if(broadcast == Broadcasting::by_place)
			{
				for(auto& o: out)
				{
				   o->send(src);
				}
			}
			else if(broadcast == Broadcasting::parallel)
			{
				std::vector<std::thread> vt;

				for(auto cid = 0; cid < out.size(); ++cid)
				{
					vt.push_back(std::thread([this, &cid, &src]()
											{
												this->out[cid]->send(src);
											}));
				}

				for(auto& t: vt)
				{
					if(t.joinable())
					{
						t.join();
					}
				}
			}
		}

		//  Unicast Command
		void send(T_DATA& src, const int cid)
		{
			if((0 <= cid) && (cid < out.size()))
			{
				out[cid]->send(src);
			}
		}

	protected:
		std::vector<std::shared_ptr<CHAN>> out;
	};
}
//---------------------------------------------------------------------------
#endif

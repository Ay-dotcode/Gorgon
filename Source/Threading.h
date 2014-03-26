///@file Threading.h contains threading functions.

#pragma once

#include <functional>
#include <thread>
#include <vector>

namespace Gorgon {

	/// Contains multi-threading functions and objects.
	/// For thread and mutex @see std::thread and std::mutex
	namespace Threading {
		

		/// Executes a function asynchronously. This function starts the thread immediately.
		/// There is no way to wait for the thread, stop or query its execution.
		/// @param fn the function to be executed.
		inline void RunAsync(std::function<void()> fn) {
			std::thread t(fn);
			t.detach();
		}
		
		/// Runs a function specified amount of times in parallel. threads parameter controls
		/// the amount of parallel executions. This function will return when all threads it
		/// controls finishes. The following example performs an operation over the data vector 
		/// using 4 threads. If the threads parameter is omitted, the number of threads supported
		/// by hardware is used.
		/// @code
		/// std::vector<int> data(1000);
		/// RunInParallel([&data](int threadid, int threads) {
		///    for(int i=threadid;i<data.size();i+=threads) {
		///        //... do something with data[i]
		///    }
		/// }, 4);
		/// @endcode
		/// @param fn is the function to be executed. First parameter of the function should be
		///        thread id, second is the number of threads. See the example.
		/// @param threads the number of threads to be executed.
		inline void RunInParallel(std::function<void(int, int)> fn, unsigned threads=0) {
			std::vector<std::thread> thrds;

			if(threads==0) threads=std::thread::hardware_concurrency();
			
			for(int id=0;id<threads;id++) {
				thrds.emplace_back(fn, id, threads);
			}
			
			for(int id=0;id<threads;id++) {
				thrds[id].join();
			}
		}
	}
	
}
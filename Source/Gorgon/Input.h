#pragma once

#include <assert.h>

#include "Event.h"
#include "Containers/Collection.h"

namespace Gorgon { 
	
	/// This namespace contains general input related functionality.
	/// Also check Gorgon::Keyboard and Gorgon::Mouse.
	namespace Input {
		/// A type to represent an input key
		typedef int Key;
		
	}
}

#ifdef nononee	

	namespace keyboard { 
		extern std::map<Key, bool> PressedKeys;


		////Types of keyboard events
		class Event {
		public:
			enum Type {
				////A character is typed (O/S controlled, i.e. repeating keys)
				Char,
				////A key is pressed
				Down,
				////A key is released
				Up
			} event;

			Key keycode;

			Event() { }
			Event(Type event, int keycode) : event(event), keycode(keycode) { }

			bool operator ==(const Event &r) const {
				return (event==r.event)&&(keycode==r.keycode);
			}
		};
	}
namespace gge { namespace input {
	namespace keyboard {
		extern utils::ConsumableEvent<utils::Empty, Event> Events;

		//You have to use start to receive events
		class EventRepeater {
		public:
			EventRepeater(unsigned timeout, std::function<void(Key key)> handler, std::string keys="") : 
				timeout(timeout), timeleft(0), keytoken(0), repeattoken(0), handler(handler)
			{
				for(auto &c : keys) {
					this->keys.push_back(c);
				}
			}

			void AddKey(Key key) {
				if(std::find(keys.begin(), keys.end(), key)==keys.end()) {
					keys.push_back(key);
				}
			}

			void RemoveKey(Key key) {
				auto it=std::find(keys.begin(), keys.end(), key);

				if(it!=keys.end())
					keys.erase(it);
			}

			void Reset();

			void Set() {
				timeleft=timeout;
			}

			void SetTimeout(unsigned timeout) {
				if(timeleft>0) {
					timeleft+=timeout-this->timeout;
					if(timeleft<0) timeleft=0;
				}
				this->timeout=timeout;
			}

			unsigned GetTimeout() const {
				return timeout;
			}

			void Start();

			void Stop();

			bool ProcessKey(Event event);

			void Tick();

		private:
			std::vector<Key> keys;
			int timeleft;
			int lasttick;
			int timeout;
			std::function<void(Key key)> handler;
			utils::ConsumableEvent<utils::Empty, Event>::Token keytoken;
			utils::EventChain<>::Token repeattoken;
		};
	}

} }
#endif

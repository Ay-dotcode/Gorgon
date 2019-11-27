#pragma once

#include "../Event.h"
#include "../Animation.h"
#include "Keyboard.h"

namespace Gorgon { namespace Input {

    namespace internal {
        struct eventunregisterhelper {
            virtual ~eventunregisterhelper() { }
            virtual void unregister() = 0;
        };

        template<class T_>
        struct eventunregisterer : public eventunregisterhelper {
            eventunregisterer(T_ &event, EventToken token) : event(event), token(token) {
            }

            virtual void unregister() override {
                event.Unregister(token);
            }

            T_ &event;
            EventToken token;
        };
    }

    /**
     * This class simplifies the use of repeated keystrokes when a key is pressed. It can
     * work automatically with a KeyEvent or manually when its functions is called. Every
     * time a key repeats the associated event will be fired with the key. If a key is
     * repeated multiple times in a single frame, the Repeat event will be fired that many
     * times one after another. Initial delay, repeat interval, and repeat acceleration can
     * be modified to customize behavior of the repeater. KeyRepeater can repeat multiple
     * keys. KeyRepeater can also be used with mouse keys. However, as of now, mouse keys
     * should be pressed and released manually.
     * 
     * KeyRepeater is also an animation, it is bound with a timer, and effected by the
     * active animation governor.
     */
    class KeyRepeater : public Animation::Base {
    public:



        /// Disable copy constructor
        KeyRepeater(const KeyRepeater &) = delete;

        /// Destructor
        ~KeyRepeater() {
            if(token)
                token->unregister();

            delete token;
        }

        /// Registers the given key to be repeated.
        void Register(Key key);

        /// Presses a key, effectively simulating keydown. Depeding on the settings, this may
        /// trigger keypress instantly.
        void Press(Key key);

        /// Releases a key.
        void Release(Key key);

        /// Checks if a key is pressed
        bool IsPressed(Key key) const;
        
        /// Sets whether the key should be pressed immediately
        void SetInstantPress(bool value);

        /// Sets if the key should be pressed right after the key is released.
        void SetPressOnRelease(bool value);

        /// Sets the initial delay before the first (or second if instant press is on) key is
        /// repeated in milliseconds. Set 0 to disable.
        void SetInitialDelay(int value);

        /// Repeat delay between successive press events in milliseconds. This is the initial 
        /// value and can be altered by acceleration.
        void SetPressDelay(int value);

        /// Change in repeat delay per repeat
        void SetPressDelayAcceleration(int value);

        /// Final delay between press events in milliseconds. Effective only when acceleration is 
        /// set. When the delay reaches this value, acceleration will be disabled and the key will
        /// be repeated at this speed.
        void SetFinalDelay(int value);

        virtual bool Progress(unsigned& leftover) override;

        virtual int GetDuration() const override;

        /// Press event that is called everytime a key is pressed.
        Event<KeyRepeater, Key> Repeat;

    private:

        internal::eventunregisterhelper *token = nullptr;

        //key to last repeat time
        std::map<Key, unsigned long> pressedkeys;
    };

} }


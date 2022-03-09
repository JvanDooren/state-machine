#include <iostream>
#include <state_machine.hh>

using namespace Core;

template<typename EnumEventType>
class DerivedEvent : public Event<EnumEventType> {
private:
    int _d;
public:
    DerivedEvent(EnumEventType t) : Event<EnumEventType>(t), _d(10) {}
    virtual ~DerivedEvent() = default;

    [[nodiscard]] int get() const noexcept {
        return _d;
    }
};


class VendingMachine {
public:
    enum class State: uint8_t {
        Idle = 0,
        Dispensing,
        Servicing,
        Last,
    };

    static const char* stateToString(const State s) {
        switch (s) {
            case State::Idle: return "Idle";
            case State::Dispensing: return "Dispensing";
            case State::Servicing: return "Servicing";
            default:
                throw std::invalid_argument("Untranslated state");
        }
    };

    enum class EventType: uint8_t {
        TeaButtonPushed = 0,
        CupFull,
        AbortButtonPushed,
        ServiceButtonPushed,
    };

    static const char* eventToString(const EventType e) {
        switch (e) {
            case EventType::TeaButtonPushed: return "TeaButtonPushed";
            case EventType::CupFull: return "CupFull";
            case EventType::AbortButtonPushed: return "AbortButtonPushed";
            case EventType::ServiceButtonPushed : return "ServiceButtonPushed";
            default:
                throw std::invalid_argument("Untranslated event");
        }
    };

    void teaSelected() noexcept {
        Event evt(EventType::TeaButtonPushed);
        _machine.handle(evt);
    }

    void abort() noexcept {
        DerivedEvent evt(EventType::AbortButtonPushed);
        _machine.handle(evt);
    }
private:
    class MyContext {

    };

    [[nodiscard]] bool _releaseLock(const MyContext& context, const Event<EventType>& event) noexcept {
        std::cout << "releasing lock" << std::endl;
        return true;
    }

    [[nodiscard]] bool _dispenseTea(const MyContext& context, const Event<EventType>& event) noexcept {
        std::cout << "dispensing tea" << std::endl;
        return true;
    }

    [[nodiscard]] bool _onCupFull(const MyContext& context, const Event<EventType>& event) noexcept {
        std::cout << "cup full handler" << std::endl;
        return true;
    }

    [[nodiscard]] bool _onAbort(const MyContext& context, const Event<EventType>& event) noexcept {
        std::cout << "abort handler" << std::endl;
        auto dre = dynamic_cast<const DerivedEvent<EventType>&>(event);
        std::cout << "derived event " << dre.get() << std::endl;
        return true;
    }

    StateMachine<VendingMachine, MyContext> _machine;
public:
    VendingMachine() :
        _machine(State::Idle,
            {
                {
                    State::Idle, {
                        {EventType::TeaButtonPushed, State::Dispensing, State::Idle, [&](const MyContext& c, const Event<EventType>& e) { return _dispenseTea(c, e); }}
                        ,{EventType::ServiceButtonPushed, State::Servicing, State::Idle, [&](const MyContext& c, const Event<EventType>& e) { return _releaseLock(c, e); }}
                    }
                },
                {
                    State::Dispensing, {
                        {EventType::CupFull, State::Idle, State::Idle, [&](const MyContext& c, const Event<EventType>& e) { return _onCupFull(c, e); }}
                        ,{EventType::AbortButtonPushed, State::Idle, State::Idle, [&](const MyContext& c, const Event<EventType>& e) { return _onAbort(c, e); }}
                    }
                },
                {
                    State::Servicing, {
                        {EventType::TeaButtonPushed, State::Dispensing, State::Idle, [&](const MyContext& c, const Event<EventType>& e) { return _dispenseTea(c, e); }}
                    }
                }
            }
        ) {}
};

int main() {
    VendingMachine bla;
    bla.teaSelected();
    bla.abort();
}

#pragma once

#include <event.hh>
#include <functional>

namespace Core {

template <typename EnumStateType, typename EnumEventType, class Context>
class StateTransition {
private:
    using Handler = std::function<bool(const Context&, const Event<EnumEventType>&)>;
    const EnumEventType _eventType;
    const EnumStateType _successState;
    const EnumStateType _failureState;
    Handler _eventHandler;
public:
    StateTransition(const EnumEventType eventType, const EnumStateType successState, const EnumStateType failureState, Handler&& op)
        : _eventType(eventType)
        , _successState(successState)
        , _failureState(failureState)
        , _eventHandler(std::move(op)) {}

    [[nodiscard]] EnumEventType eventType() const noexcept {
        return _eventType;
    }

    void handle(const Context& ctx, EnumStateType& currentState, const Event<EnumEventType>& event) const noexcept {
        currentState = _successState;
        if (!_eventHandler(ctx, event)) {
            currentState = _failureState;
        }
    }
};

}

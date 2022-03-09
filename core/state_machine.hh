#pragma once

#include <state_transition.hh>
#include <list>
#include <map>
#include <mutex>
#include <sstream>
#include <type_traits>

namespace Core {

template<class Owner, class Context>
class StateMachine {
private:
    using StateEnumType = typename Owner::State;
    using EventEnumType = typename Owner::EventType;
    static_assert(std::is_enum_v<StateEnumType>);
    static_assert(std::is_enum_v<EventEnumType>);

    using StateTransitionMap = std::map<StateEnumType, std::list<StateTransition<StateEnumType, EventEnumType, Context>>>;

private:
    StateEnumType _currentState;
    StateTransitionMap _stateTransitions = {};

    Context _ctx;
    std::mutex _m;
public:
    StateMachine(StateEnumType currState, StateTransitionMap map) : _currentState(currState), _stateTransitions(std::move(map)) {
        _validate();
    }
private:
    void _validate() const {
        std::stringstream s;
        if (_stateTransitions.size() != (size_t)StateEnumType::Last) {
            s << "Number of states used in state transitions map " << _stateTransitions.size() << " doesn't match number of states " << (size_t)StateEnumType::Last;
            throw std::logic_error(s.str());
        }
        for (const auto& row : _stateTransitions) {
            if (row.second.empty()) {
                s << "Found empty transition list for state " << Owner::stateToString(row.first);
                throw std::logic_error(s.str());
            }
            else {
                // Iterate over the list and store the frequency of each element in map
                std::map<EventEnumType, int> countMap;
                for (const auto& transition : row.second) {
                    auto result = countMap.insert(std::pair<EventEnumType, int>(transition.eventType(), 1));
                    if (result.second == false) {
                        result.first->second++;
                    }
                }
                
                // Remove the elements from Map which has 1 frequency count
                for (auto it = countMap.begin() ; it != countMap.end() ;) {
                    std::cout << " it->second: " << it->second << std::endl;
                    if (it->second == 1)
                        it = countMap.erase(it);
                    else
                        it++;
                }

                if (!countMap.empty()) {
                    for (const auto& eventRow : countMap) {
                        s << std::endl << "In state '" << Owner::stateToString(row.first) << "' found event '" << Owner::eventToString(eventRow.first) << "' exactly " << eventRow.second << " times, expected only once";
                    }

                    throw std::logic_error(s.str());
                }
            }
        }
    }
public:
    template <typename Event>
    void handle(const Event& event) noexcept {
        std::scoped_lock lock(_m);
        auto transitions = _stateTransitions[_currentState];
        // check if there is a registration for this event
        for (const auto& transition : transitions) {
            if (event.isOfType(transition.eventType())) {
                transition.handle(_ctx, _currentState, event);
                break;
            }
        }
    }
};

}

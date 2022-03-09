#pragma once

namespace Core {

template<typename EnumEventType>
class Event {
private:
    EnumEventType _evtEnum;
public:
    Event(EnumEventType e) : _evtEnum(e) {}
    virtual ~Event() = default;

    [[nodiscard]] bool isOfType(EnumEventType e) const noexcept {
        return e == _evtEnum;
    }
};

}

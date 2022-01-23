/**
 * @file Statemachine.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 19-11-2021
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace cpp_event_framework
{
/**
 * @brief State properties flags
 */
enum class EStateFlags
{
    kNone = 0,
    kHistory = 1
};
/**
 * @brief |= operator for state flags
 *
 * @param lhs
 * @param rhs
 * @return EStateFlags&
 */
inline EStateFlags operator|(EStateFlags lhs, EStateFlags rhs)
{
    using T = std::underlying_type_t<EStateFlags>;
    return static_cast<EStateFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}
/**
 * @brief |= operator for state flags
 *
 * @param lhs
 * @param rhs
 * @return EStateFlags&
 */
inline EStateFlags& operator|=(EStateFlags& lhs, EStateFlags rhs)
{
    lhs = lhs | rhs;
    return lhs;
}
/**
 * @brief & operator for state flags
 *
 * @param lhs
 * @param rhs
 * @return EStateFlags&
 */
inline EStateFlags operator&(EStateFlags lhs, EStateFlags rhs)
{
    using T = std::underlying_type_t<EStateFlags>;
    return static_cast<EStateFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}
/**
 * @brief &= operator for state flags
 *
 * @param lhs
 * @param rhs
 * @return EStateFlags&
 */
inline EStateFlags& operator&=(EStateFlags& lhs, EStateFlags rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

/**
 * @brief Statemachine implementation
 *
 */
template <typename OwnerType, typename EventType>
class Statemachine
{
public:
    class State;

    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<Statemachine>;

    /**
     * @brief Statemachine owner type
     *
     */
    using Owner = OwnerType;
    /**
     * @brief Statemachine event type
     *
     */
    using Event = EventType;
    /**
     * @brief State property flags
     *
     */
    using EFlags = EStateFlags;
    /**
     * @brief State pointer (used in many function signatures)
     *
     */
    using StatePtr = const State*;

    /**
     * @brief State machine transition class, used internally
     */
    class Transition
    {
    public:
        /**
         * @brief Type of action handler
         *
         */
        using ActionType = void (Owner::*)(Event);

        /**
         * @brief Transition target
         *
         */
        StatePtr target_ = nullptr;
        /**
         * @brief Optional transition action
         *
         */
        ActionType action_ = nullptr;

        /**
         * @brief Construct a new Statemachine Transition object
         *
         */
        Transition() = default;
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         */
        constexpr Transition(const State& target) : target_(&target)
        {
        }
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         * @param action Transition action
         */
        constexpr Transition(const State& target, ActionType action) noexcept : target_(&target), action_(action)
        {
        }
    };

    /**
     * @brief Statemachine state
     *
     */
    class State
    {
    public:
        /**
         * @brief Type of on_entry / on_exit handler
         *
         */
        using EntryExitType = void (Owner::*)(StatePtr);
        /**
         * @brief Type of event handler
         *
         */
        using HandlerType = Transition (Owner::*)(StatePtr, Event);

        /**
         * @brief Flags indicating state properties
         *
         */
        const EFlags flags_ = EFlags::kNone;
        /**
         * @brief Optional parent state
         *
         */
        StatePtr parent_ = nullptr;
        /**
         * @brief Optional initial substate
         *
         */
        StatePtr initial_ = nullptr;
        /**
         * @brief Statename, optional, useful for logging
         *
         */
        const char* name_ = "Unnamed";

        /**
         * @brief Optional entry action
         *
         */
        EntryExitType on_entry_ = nullptr;
        /**
         * @brief Optional exit action
         *
         */
        EntryExitType on_exit_ = nullptr;
        /**
         * @brief Statemachine handler, must be assigned
         */
        HandlerType handler_ = nullptr;

        /**
         * @brief Construct a new Statemachine State object
         *
         */
        constexpr State(const char* name, HandlerType handler, StatePtr parent = nullptr, StatePtr initial = nullptr,
                        EntryExitType on_entry = nullptr, EntryExitType on_exit = nullptr,
                        EFlags flags = EFlags::kNone) noexcept
            : flags_(flags)
            , parent_(parent)
            , initial_(initial)
            , name_(name)
            , on_entry_(std::move(on_entry))
            , on_exit_(std::move(on_exit))
            , handler_(std::move(handler))
        {
        }
    };

    /**
     * @brief State is entered (useful for logging)
     *
     */
    std::function<void(StatePtr)> on_state_entry_;
    /**
     * @brief State is left (useful for logging)
     *
     */
    std::function<void(StatePtr)> on_state_exit_;
    /**
     * @brief Event is passed to a state (useful for logging)
     *
     */
    std::function<void(StatePtr, Event)> on_handle_event_;
    /**
     * @brief Unhandled event callback, fired when top-level state does not handle
     * event
     *
     */
    std::function<void(Event)> on_unhandled_event_;
    /**
     * @brief Deferred event callback, fired event deferral is requestd
     *
     */
    std::function<void(StatePtr, Event)> on_defer_event_;

    /**
     * @brief Construct a new Statemachine object
     *
     */
    Statemachine() = default;
    /**
     * @brief Copy constructor
     *
     * @param rhs
     */
    Statemachine(const Statemachine& rhs) = default;
    /**
     * @brief Move constructor
     *
     * @param rhs
     */
    Statemachine(Statemachine&& rhs) noexcept = default;

    /**
     * @brief Destroy the Statemachine object
     *
     */
    ~Statemachine() = default;

    /**
     * @brief Copy operator
     *
     * @param rhs
     * @return Statemachine&
     */
    Statemachine& operator=(const Statemachine& rhs) = default;
    /**
     * @brief Move operator
     *
     * @param rhs
     * @return Statemachine&
     */
    Statemachine& operator=(Statemachine&& rhs) noexcept = default;

    /**
     * @brief Initialize statemachine with owner and name.
     *
     * @param owner Statemachine owner
     * @param name Statemachine name, useful for logging
     * @param initial Initial state
     */
    void Init(Owner* owner, std::string name, const StatePtr initial)
    {
        name_ = std::move(name);
        owner_ = owner;
        initial_[nullptr] = initial;
    }
    /**
     * @brief Start statemachine, enter initial state
     *
     */
    void Start()
    {
        current_state_ = &kInTransition;
        EnterStatesFromDownTo(nullptr, initial_[nullptr]);
    }

    /**
     * @brief Synchronously react to an event
     *
     * @param event Event
     */
    void React(Event event)
    {
        assert(current_state_ != nullptr);
        assert(!working_);
        working_ = true;

        Transition transition;
        const auto* s = current_state_;

        do
        {
            if (on_handle_event_)
            {
                on_handle_event_(s, event);
            }
            transition = (owner_->*s->handler_)(s, event);

            if (transition.target_ == &kDeferEvent)
            {
                assert(on_defer_event_ != nullptr);
                on_defer_event_(s, event);
                working_ = false;
                return;
            }

            s = s->parent_;
        } while ((transition.target_ == &kUnhandled) && (s != nullptr));

        if ((transition.target_ != &kUnhandled))
        {
            if (transition.target_ != &kNone)
            {
                const auto* common_parent = FindCommonParent(current_state_, transition.target_);

                const auto* old_state = current_state_;
                current_state_ = &kInTransition;

                ExitStatesFromUpTo(old_state, common_parent);

                if (transition.action_ != nullptr)
                {
                    (owner_->*transition.action_)(event);
                }

                EnterStatesFromDownTo(common_parent, transition.target_);
            }
            else
            {
                // No transition
                if (transition.action_ != nullptr)
                {
                    (owner_->*transition.action_)(event);
                }
            }
        }
        else
        {
            if (on_unhandled_event_)
            {
                on_unhandled_event_(event);
            }
        }

        working_ = false;
    }

    /**
     * @brief Returns current state
     *
     * @return const StatemachineState*
     */
    StatePtr CurrentState() const
    {
        return current_state_;
    }

    /**
     * @brief Returns name
     *
     * @return const std::string&
     */
    [[nodiscard]] const std::string& Name() const
    {
        return name_;
    }

    /**
     * @brief Event was not handled in this state, shall be passed to parent state
     *
     * @return Transition
     */
    static Transition UnhandledEvent()
    {
        return TransitionTo(kUnhandled);
    }
    /**
     * @brief Defer event until state is exited
     *
     * @return Transition
     */
    static Transition DeferEvent()
    {
        return TransitionTo(kDeferEvent);
    }
    /**
     * @brief Event was handled, but no transition shall be executed, with
     * optional action
     *
     * @param action Action to execute
     * @return Transition
     */
    static Transition NoTransition(typename Transition::ActionType action = nullptr)
    {
        return TransitionTo(kNone, action);
    }
    /**
     * @brief Create transition to target state, with optional action
     *
     * @param target Target state
     * @param action Action to execute on transition
     * @return Transition
     */
    static Transition TransitionTo(const State& target, typename Transition::ActionType action = nullptr)
    {
        if (action == nullptr)
        {
            return Transition(target);
        }
        return Transition(target, action);
    }

private:
    StatePtr current_state_ = nullptr;
    bool working_ = false;
    Owner* owner_ = nullptr;
    std::string name_;
    std::map<StatePtr, StatePtr> initial_;

    static const State kNone;
    static const State kUnhandled;
    static const State kInTransition;
    static const State kDeferEvent;

    void SetInitialState(StatePtr owner, StatePtr initial)
    {
        if ((owner->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            initial_[owner] = initial;
        }
        else
        {
            // ignore
        }
    }

    StatePtr GetInitialState(StatePtr owner) const
    {
        if ((owner->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            auto search = initial_.find(owner);
            if (search != initial_.end())
            {
                return search->second;
            }
        }

        return owner->initial_;
    }

    void ExitStatesFromUpTo(StatePtr from, StatePtr top)
    {
        const auto* s = from;

        while (s != top)
        {
            // Save history state
            if (s->parent_ != nullptr)
            {
                if ((s->parent_->flags_ & EFlags::kHistory) != EFlags::kNone)
                {
                    SetInitialState(s->parent_, s);
                }
            }

            if (on_state_exit_ != nullptr)
            {
                on_state_exit_(s);
            }

            if (s->on_exit_ != nullptr)
            {
                (owner_->*s->on_exit_)(s);
            }

            s = s->parent_;
        };
    }

    void EnterState(StatePtr s) const
    {
        if (on_state_entry_ != nullptr)
        {
            on_state_entry_(s);
        }

        if (s->on_entry_ != nullptr)
        {
            (owner_->*s->on_entry_)(s);
        }
    }

    void EnterStatesFromDownToRecursive(StatePtr top, StatePtr target)
    {
        if (top != target)
        {
            if (target->parent_ != nullptr)
            {
                EnterStatesFromDownTo(top, target->parent_);
            }
            EnterState(target);
        }
    }
    void EnterStatesFromDownTo(StatePtr top, StatePtr target)
    {
        EnterStatesFromDownToRecursive(top, target->parent_);

        if (GetInitialState(target) != nullptr)
        {
            EnterState(target);

            const auto* s = GetInitialState(target);
            const auto* t = s;
            while (s != nullptr)
            {
                t = s;
                EnterState(s);
                s = GetInitialState(s);
            }
            current_state_ = t;
        }
        else
        {
            EnterState(target);
            current_state_ = target;
        }
    }
    static StatePtr FindCommonParent(StatePtr s1, StatePtr s2)
    {
        auto* s = s1->parent_;

        while (s != nullptr)
        {
            auto* o = s2->parent_;

            while (o != nullptr)
            {
                if (o == s)
                {
                    return o;
                }

                o = o->parent_;
            }

            s = s->parent_;
        }

        return nullptr;
    }
};

template <typename Owner, typename Event>
const typename Statemachine<Owner, Event>::State
    Statemachine<Owner, Event>::kNone = typename Statemachine<Owner, Event>::State("None", nullptr);
template <typename Owner, typename Event>
const typename Statemachine<Owner, Event>::State
    Statemachine<Owner, Event>::kUnhandled = typename Statemachine<Owner, Event>::State("Unhandled", nullptr);
template <typename Owner, typename Event>
const typename Statemachine<Owner, Event>::State
    Statemachine<Owner, Event>::kInTransition = typename Statemachine<Owner, Event>::State("InTransition", nullptr);
template <typename Owner, typename Event>
const typename Statemachine<Owner, Event>::State
    Statemachine<Owner, Event>::kDeferEvent = typename Statemachine<Owner, Event>::State("Defer", nullptr);
} // namespace cpp_event_framework

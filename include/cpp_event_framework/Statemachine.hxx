/**
 * @file Statemachine.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 19-11-2021
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <array>
#include <cassert>
#include <map>
#include <memory>
#include <span>
#include <string>

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
inline constexpr EStateFlags operator|(EStateFlags lhs, EStateFlags rhs)
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
inline constexpr EStateFlags& operator|=(EStateFlags& lhs, EStateFlags rhs)
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
inline constexpr EStateFlags operator&(EStateFlags lhs, EStateFlags rhs)
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
inline constexpr EStateFlags& operator&=(EStateFlags& lhs, EStateFlags rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

/**
 * @brief Statemachine implementation
 *
 */
template <typename ImplType, typename EventType>
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
     * @brief Unique pointer alias
     *
     */
    using UPtr = std::unique_ptr<Statemachine>;
    /**
     * @brief Weak pointer alias
     *
     */
    using WPtr = std::weak_ptr<Statemachine>;
    /**
     * @brief Constant statemachine referenace
     *
     */
    using Ref = const Statemachine&;

    /**
     * @brief Statemachine implementation type
     *
     */
    using Impl = ImplType;
    /**
     * @brief Statemachine implementation pointer type
     *
     */
    using ImplPtr = ImplType*;
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
     * @brief State reference (used in many function signatures)
     *
     */
    using StateRef = const State&;

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
        using ActionType = void (ImplType::*)(Event);
        /**
         * @brief Type of action handler
         *
         */
        using DelegateActionType = void (*)(ImplPtr, Event);

        /**
         * @brief Action container type
         *
         */
        template <size_t num>
        class ActionContainer : public std::array<const ActionType, num>
        {
        };

        /**
         * @brief Transition target
         *
         */
        StatePtr target_ = nullptr;
        /**
         * @brief Execute transition actions
         *
         * @param impl
         * @param event
         */
        void ExecuteActions(ImplPtr impl, Event event)
        {
            if (delegate_action_ != nullptr)
            {
                delegate_action_(impl, event);
            }
            for (const auto& action : actions_)
            {
                (impl->*action)(event);
            }
        }

        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         */
        constexpr explicit Transition(StateRef target) : target_(&target)
        {
        }
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         */
        constexpr Transition(StateRef target, DelegateActionType action) : target_(&target), delegate_action_(action)
        {
        }
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         * @param action Transition action
         */
        constexpr Transition(StateRef target, ActionType action)
            : target_(&target), single_action_(action), actions_(&single_action_, 1)
        {
        }
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         * @param actions Transition actions
         */
        constexpr Transition(StateRef target, std::span<const ActionType> actions) noexcept
            : target_(&target), actions_(actions)
        {
        }

    private:
        /**
         * @brief Optional single transition action
         *
         * Provides storage for single transition actions
         */
        ActionType single_action_ = nullptr;
        /**
         * @brief Optional single lambda transition action
         *
         * std::function<> cannot be used here - not longer possible to store Transition in RO section!
         */
        DelegateActionType delegate_action_ = nullptr;
        /**
         * @brief Optional transition actions
         *
         */
        std::span<const ActionType> actions_;
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
        using EntryExitType = void (Impl::*)();
        /**
         * @brief Type of event handler
         *
         */
        using HandlerType = Transition (*)(ImplPtr, Event);

        /**
         * @brief Flags indicating state properties
         *
         */
        const EFlags flags_ = EFlags::kNone;
        /**
         * @brief Optional parent state
         *
         */
        StatePtr const parent_ = nullptr;
        /**
         * @brief Optional initial substate
         *
         */
        StatePtr const initial_ = nullptr;

        /**
         * @brief Optional entry action
         *
         */
        EntryExitType const on_entry_ = nullptr;
        /**
         * @brief Optional exit action
         *
         */
        EntryExitType const on_exit_ = nullptr;
        /**
         * @brief Statemachine handler, must be assigned
         */
        HandlerType const handler_ = nullptr;

        /**
         * @brief Construct a new Statemachine State object
         *
         */
        constexpr State(const char* name, HandlerType handler, StatePtr parent = nullptr, StatePtr initial = nullptr,
                        EntryExitType on_entry = nullptr, EntryExitType on_exit = nullptr) noexcept
            : State(name, handler, parent, initial, on_entry, on_exit, EFlags::kNone)
        {
        }

        /**
         * @brief Statemachine name
         *
         * @return const char*
         */
        [[nodiscard]] const char* Name() const
        {
            return name_;
        }

    protected:
        /**
         * @brief Construct a new Statemachine State object
         *
         */
        constexpr State(const char* name, HandlerType handler, StatePtr parent, StatePtr initial,
                        EntryExitType on_entry, EntryExitType on_exit, EFlags flags) noexcept
            : flags_(flags)
            , parent_(parent)
            , initial_(initial)
            , on_entry_(on_entry)
            , on_exit_(on_exit)
            , handler_(handler)
            , name_(name)
        {
        }

    private:
        /**
         * @brief Statename, optional, useful for logging
         *
         */
        const char* name_ = "Unnamed";
    };

    /**
     * @brief History statemachine state
     *
     */
    class HistoryState : public State
    {
    public:
        /**
         * @brief Construct a new Statemachine State object
         *
         */
        constexpr HistoryState(const char* name, typename State::HandlerType handler, StatePtr parent = nullptr,
                               StatePtr initial = nullptr, typename State::EntryExitType on_entry = nullptr,
                               typename State::EntryExitType on_exit = nullptr) noexcept
            : State(name, handler, parent, initial, on_entry, on_exit, EFlags::kHistory)
        {
        }
    };

    /**
     * @brief State is entered (useful for logging)
     *
     */
    void (*on_state_entry_)(Ref, StateRef) = nullptr;
    /**
     * @brief State is left (useful for logging)
     *
     */
    void (*on_state_exit_)(Ref, StateRef) = nullptr;
    /**
     * @brief Event is passed to a state (useful for logging)
     *
     */
    void (*on_handle_event_)(Ref, StateRef, Event) = nullptr;
    /**
     * @brief Unhandled event callback, fired when top-level state does not handle
     * event
     *
     */
    void (*on_unhandled_event_)(Ref, StateRef, Event) = nullptr;
    /**
     * @brief Deferred event callback, fired event deferral is requested
     *
     */
    void (*on_defer_event_)(Ref, StateRef, Event) = nullptr;
    /**
     * @brief Deferred event callback, fired event recall is requested
     *
     */
    void (*on_recall_deferred_events_)(Ref, StateRef) = nullptr;

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
     * @brief Copy assignment
     *
     * @param rhs
     * @return Statemachine&
     */
    Statemachine& operator=(const Statemachine& rhs) = default;
    /**
     * @brief Move assignment
     *
     * @param rhs
     * @return Statemachine&
     */
    Statemachine& operator=(Statemachine&& rhs) noexcept = default;

    /**
     * @brief Initialize statemachine with impl, name and initial state.
     *
     * @param impl Statemachine implementation
     * @param name Statemachine name, useful for logging
     * @param initial Initial state
     */
    void Init(ImplPtr impl, std::string name, const State* initial)
    {
        assert(impl != nullptr);
        assert(initial != nullptr);
        name_ = std::move(name);
        impl_ = impl;
        initial_[nullptr] = initial;
    }
    /**
     * @brief Start statemachine, enter initial state
     *
     */
    void Start()
    {
        assert(impl_ != nullptr);             // Most probably you forgot to call Init()
        assert(initial_[nullptr] != nullptr); // Most probably you forgot to call Init()
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
        assert(current_state_ != nullptr); // Most probably you forgot to call Start()
        assert(!working_);                 // Most probably you are recursively calling React()
        working_ = true;

        Transition transition(kInTransition);
        const auto* start = current_state_;
        const auto* s = current_state_;

        do
        {
            if (on_handle_event_ != nullptr)
            {
                on_handle_event_(*this, *s, event);
            }
            transition = s->handler_(impl_, event);

            if (transition.target_ == &kDeferEvent)
            {
                assert(on_defer_event_ != nullptr);
                on_defer_event_(*this, *s, event);
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
                transition.ExecuteActions(impl_, event);
                EnterStatesFromDownTo(common_parent, transition.target_);
            }
            else
            {
                // No transition
                transition.ExecuteActions(impl_, event);
            }
        }
        else
        {
            if (on_unhandled_event_ != nullptr)
            {
                on_unhandled_event_(*this, *start, event);
            }
        }

        working_ = false;
    }

    /**
     * @brief Recall deferred events
     *
     */
    void RecallEvents()
    {
        assert(on_recall_deferred_events_ != nullptr);
        on_recall_deferred_events_(*this, *current_state_);
    }

    /**
     * @brief Returns current state
     *
     * @return StatePtr State pointer
     */
    StatePtr CurrentState() const
    {
        return current_state_;
    }

    /**
     * @brief Implementation
     *
     * @return ImplPtr Implementation pointer
     */
    ImplPtr Implementation() const
    {
        return impl_;
    }

    /**
     * @brief Returns name
     *
     * @return const std::string& Statemachine name
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
    static inline Transition UnhandledEvent()
    {
        return Transition(kUnhandled);
    }
    /**
     * @brief Defer event until state is exited
     *
     * @return Transition
     */
    static inline Transition DeferEvent()
    {
        return Transition(kDeferEvent);
    }
    /**
     * @brief Event was handled, but no transition shall be executed, with
     * optional action
     *
     * @return Transition
     */
    static inline Transition NoTransition()
    {
        return Transition(kNone);
    }
    /**
     * @brief Event was handled, but no transition shall be executed, with
     * optional action
     *
     * @param action Action to execute
     * @return Transition
     */
    static inline Transition NoTransition(typename Transition::DelegateActionType action)
    {
        return Transition(kNone, action);
    }
    /**
     * @brief Create transition to target state, with optional action
     *
     * @param target Target state
     * @return Transition
     */
    static inline Transition TransitionTo(StateRef target)
    {
        return Transition(target);
    }
    /**
     * @brief Create transition to target state, with optional action
     *
     * @param target Target state
     * @param action Action to execute on transition
     * @return Transition
     */
    static inline Transition TransitionTo(StateRef target, typename Transition::DelegateActionType action)
    {
        return Transition(target, action);
    }

    /**
     * @brief No transition
     */
    static const State kNone;
    /**
     * @brief Unhandled transition
     */
    static const State kUnhandled;
    /**
     * @brief Event deferral request
     */
    static const State kDeferEvent;

private:
    StatePtr current_state_ = nullptr;
    bool working_ = false;
    ImplPtr impl_ = nullptr;
    std::string name_;
    std::map<StatePtr, StatePtr> initial_;

    static const State kInTransition;

    void SetInitialState(StatePtr impl, StatePtr initial)
    {
        if ((impl->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            initial_[impl] = initial;
        }
        else
        {
            // ignore
        }
    }

    StatePtr GetInitialState(StatePtr impl) const
    {
        if ((impl->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            auto search = initial_.find(impl);
            if (search != initial_.end())
            {
                return search->second;
            }
        }

        return impl->initial_;
    }

    void ExitStatesFromUpTo(StatePtr from, StatePtr top)
    {
        const auto* state = from;

        while (state != top)
        {
            // Save history state
            if (state->parent_ != nullptr)
            {
                if ((state->parent_->flags_ & EFlags::kHistory) != EFlags::kNone)
                {
                    SetInitialState(state->parent_, state);
                }
            }

            if (on_state_exit_ != nullptr)
            {
                on_state_exit_(*this, *state);
            }

            if (state->on_exit_ != nullptr)
            {
                (impl_->*state->on_exit_)();
            }

            state = state->parent_;
        };
    }

    void EnterState(StateRef state) const
    {
        if (on_state_entry_ != nullptr)
        {
            on_state_entry_(*this, state);
        }

        if (state.on_entry_ != nullptr)
        {
            (impl_->*state.on_entry_)();
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
            EnterState(*target);
        }
    }
    void EnterStatesFromDownTo(StatePtr top, StatePtr target)
    {
        EnterStatesFromDownToRecursive(top, target->parent_);

        if (GetInitialState(target) != nullptr)
        {
            EnterState(*target);

            const auto* state = GetInitialState(target);
            const auto* current_target = state;
            while (state != nullptr)
            {
                current_target = state;
                EnterState(*state);
                state = GetInitialState(state);
            }
            current_state_ = current_target;
        }
        else
        {
            EnterState(*target);
            current_state_ = target;
        }
    }
    static StatePtr FindCommonParent(StatePtr state1, StatePtr state2)
    {
        auto* state = state1->parent_;

        while (state != nullptr)
        {
            auto* parent = state2->parent_;

            while (parent != nullptr)
            {
                if (parent == state)
                {
                    return parent;
                }

                parent = parent->parent_;
            }

            state = state->parent_;
        }

        return nullptr;
    }
};

template <typename Impl, typename Event>
const typename Statemachine<Impl, Event>::State Statemachine<Impl, Event>::kNone =
    typename Statemachine<Impl, Event>::State("None", nullptr);
template <typename Impl, typename Event>
const typename Statemachine<Impl, Event>::State Statemachine<Impl, Event>::kUnhandled =
    typename Statemachine<Impl, Event>::State("Unhandled", nullptr);
template <typename Impl, typename Event>
const typename Statemachine<Impl, Event>::State Statemachine<Impl, Event>::kInTransition =
    typename Statemachine<Impl, Event>::State("InTransition", nullptr);
template <typename Impl, typename Event>
const typename Statemachine<Impl, Event>::State Statemachine<Impl, Event>::kDeferEvent =
    typename Statemachine<Impl, Event>::State("Defer", nullptr);
} // namespace cpp_event_framework

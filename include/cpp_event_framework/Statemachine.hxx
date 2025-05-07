/**
 * @file Statemachine.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 19-11-2021
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <span>

#include <cpp_event_framework/Concepts.hxx>
#include <cpp_event_framework/HeapAllocator.hxx>

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
constexpr EStateFlags operator|(EStateFlags lhs, EStateFlags rhs)
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
constexpr EStateFlags& operator|=(EStateFlags& lhs, EStateFlags rhs)
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
constexpr EStateFlags operator&(EStateFlags lhs, EStateFlags rhs)
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
constexpr EStateFlags& operator&=(EStateFlags& lhs, EStateFlags rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

/**
 * @brief Statemachine implementation
 *
 * @tparam ImplType Statemachine implementation type
 * @tparam EventType Event type
 * @tparam HistoryMapAllocator Allocator for history map (only when history states are used)
 */
template <typename ImplType, typename EventType, PolymorphicAllocatorProvider HistoryMapAllocator = HeapAllocator,
          AssertionProvider AssertionProviderType = DefaultAssertionProvider>
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
     * @brief Type of action handler
     *
     */
    using ActionType = void (ImplType::*)(Event);

    /**
     * @brief State machine transition class, used internally
     */
    class Transition
    {
    private:
        // this class is for internal use by Statemachine only
        friend class Statemachine;

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
            for (const auto& action : actions_)
            {
                (impl->*action)(event);
            }
        }

        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::UnhandledEvent() instead
         *
         */
        constexpr Transition() noexcept = default;
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         */
        constexpr explicit Transition(StateRef target) noexcept : target_(&target)
        {
        }
        /**
         * @brief Construct a new Statemachine Transition object
         * Use Statemachine::TransitionTo() instead
         *
         * @param target Target state
         * @param action Transition action
         */
        constexpr Transition(StateRef target, ActionType action) noexcept
            : target_(&target)
            , single_action_(action)
            , actions_(std::span<const ActionType>(&single_action_, action != nullptr ? 1 : 0))
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

        /**
         * @brief operator=
         */
        constexpr Transition& operator=(const Transition& rhs)
        {
            if (this != &rhs)
            {
                target_ = rhs.target_;
                single_action_ = rhs.single_action_;
                if (single_action_ != nullptr)
                {
                    // internal buffer was used
                    actions_ = {&single_action_, 1};
                }
                else
                {
                    // external buffer was supplied or no action at all
                    actions_ = rhs.actions_;
                }
            }

            return *this;
        }

        /**
         * @brief Optional single transition action
         *
         * Provides storage for single transition actions
         */
        ActionType single_action_ = nullptr;
        /**
         * @brief Optional transition actions
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
         */
        using EntryExitType = void (Impl::*)(Event);
        /**
         * @brief Type of event handler
         */
        using HandlerType = Transition (*)(ImplPtr, Event);

        /**
         * @brief Flags indicating state properties
         */
        EFlags flags_ = EFlags::kNone;
        /**
         * @brief Optional parent state
         */
        StatePtr const parent_ = nullptr;
        /**
         * @brief Optional initial substate
         */
        StatePtr const initial_ = nullptr;

        /**
         * @brief Optional list of entry actions
         */
        std::span<const EntryExitType> const on_entry_;
        /**
         * @brief Optional list of exit actions
         */
        std::span<const EntryExitType> const on_exit_;
        /**
         * @brief Statemachine handler, must be assigned
         */
        HandlerType const handler_ = nullptr;

        /**
         * @brief Construct a new Statemachine State object
         */
        constexpr State(const char* name, HandlerType handler, StatePtr parent = nullptr, StatePtr initial = nullptr,
                        EntryExitType on_entry = nullptr, EntryExitType on_exit = nullptr,
                        EFlags flags = EFlags::kNone) noexcept
            : State(name, handler, parent, initial, on_entry,
                    std::span<const EntryExitType>(&on_single_entry_, on_entry != nullptr ? 1 : 0), on_exit,
                    std::span<const EntryExitType>(&on_single_exit_, on_exit != nullptr ? 1 : 0), flags)
        {
        }

        /**
         * @brief Construct a new Statemachine State object
         */
        constexpr State(const char* name, HandlerType handler, StatePtr parent, StatePtr initial,
                        std::span<const EntryExitType> on_entry, std::span<const EntryExitType> on_exit,
                        EFlags flags = EFlags::kNone) noexcept
            : State(name, handler, parent, initial, nullptr, on_entry, nullptr, on_exit, flags)
        {
        }

        /**
         * @brief operator=
         */
        constexpr State& operator=(const State& rhs)
        {
            if (this != &rhs)
            {
                name_ = rhs.name_;
                handler_ = rhs.handler_;
                parent_ = rhs.parent_;
                initial_ = rhs.initial_;
                on_single_entry_ = rhs.on_single_exit_;
                on_single_exit_ = rhs.on_single_exit_;
                flags_ = rhs.flags_;
                if (on_single_entry_ != nullptr)
                {
                    // internal buffer was used
                    on_entry_ = {&on_single_entry_, 1};
                }
                else
                {
                    // external buffer was supplied or no action at all
                    on_entry_ = rhs.on_entry_;
                }
                if (on_single_exit_ != nullptr)
                {
                    // internal buffer was used
                    on_exit_ = {&on_single_exit_, 1};
                }
                else
                {
                    // external buffer was supplied or no action at all
                    on_exit_ = rhs.on_exit_;
                }
            }

            return *this;
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

        /**
         * @brief Stream operator for logging
         */
        friend std::ostream& operator<<(std::ostream& os, StateRef state)
        {
            return os << state.Name();
        }

    private:
        constexpr State(const char* name, HandlerType handler, StatePtr parent, StatePtr initial,
                        EntryExitType on_single_entry, std::span<const EntryExitType> on_entry,
                        EntryExitType on_single_exit, std::span<const EntryExitType> on_exit, EFlags flags) noexcept
            : flags_(flags)
            , parent_(parent)
            , initial_(initial)
            , on_entry_(on_entry)
            , on_exit_(on_exit)
            , handler_(handler)
            , on_single_entry_(on_single_entry)
            , on_single_exit_(on_single_exit)
            , name_(name)
        {
        }

        EntryExitType on_single_entry_ = nullptr;
        EntryExitType on_single_exit_ = nullptr;
        const char* name_ = nullptr;
    };

    /**
     * @brief History statemachine state
     */
    class HistoryState : public State
    {
    public:
        /**
         * @brief Construct a new History State
         */
        constexpr HistoryState(const char* name, typename State::HandlerType handler, StatePtr parent = nullptr,
                               StatePtr initial = nullptr, typename State::EntryExitType on_entry = nullptr,
                               typename State::EntryExitType on_exit = nullptr) noexcept
            : State(name, handler, parent, initial, on_entry, on_exit, EFlags::kHistory)
        {
        }

        /**
         * @brief Construct a new History State
         */
        constexpr HistoryState(const char* name, typename State::HandlerType handler, StatePtr parent, StatePtr initial,
                               std::span<const typename State::EntryExitType> on_entry,
                               std::span<const typename State::EntryExitType> on_exit) noexcept
            : State(name, handler, parent, initial, on_entry, on_exit, EFlags::kHistory)
        {
        }
    };

    /**
     * @brief State is changed (useful for logging)
     */
    void (*on_state_change_)(Ref, Event, StateRef, StateRef) = nullptr;
    /**
     * @brief State is entered (useful for logging)
     */
    void (*on_state_entry_)(Ref, StateRef) = nullptr;
    /**
     * @brief State is left (useful for logging)
     */
    void (*on_state_exit_)(Ref, StateRef) = nullptr;
    /**
     * @brief Event is passed to a state (useful for logging)
     */
    void (*on_handle_event_)(Ref, StateRef, Event) = nullptr;
    /**
     * @brief Unhandled event callback, fired when top-level state does not handle
     * event
     */
    void (*on_unhandled_event_)(Ref, StateRef, Event) = nullptr;
    /**
     * @brief Deferred event callback, fired event deferral is requested
     */
    std::function<void(StateRef, Event)> on_defer_event_;
    /**
     * @brief Deferred event callback, fired event recall is requested
     */
    std::function<void(StateRef)> on_recall_deferred_events_;

    /**
     * @brief Construct a new Statemachine object
     */
    Statemachine() : initial_(HistoryMapAllocator::GetAllocator())
    {
    }
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
     */
    void Init(ImplPtr impl, const char* name)
    {
        AssertionProviderType::Assert(impl != nullptr);
        name_ = name;
        impl_ = impl;
    }
    /**
     * @brief Start statemachine, enter initial state
     *
     * @param initial Initial state
     */
    void Start(const State* initial)
    {
        AssertionProviderType::Assert(impl_ != nullptr); // Most probably you forgot to call Init()
        current_state_ = &kInTransition;
        initial_.clear();
        EnterStatesFromDownTo(nullptr, initial, {});
    }

    /**
     * @brief Synchronously react to an event
     *
     * @param event Event
     */
    void React(Event event)
    {
        AssertionProviderType::Assert(current_state_ != nullptr); // Most probably you forgot to call Start()
        AssertionProviderType::Assert(!working_);                 // Most probably you are recursively calling React()
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
                AssertionProviderType::Assert(on_defer_event_ != nullptr);
                on_defer_event_(*s, event);
                working_ = false;
                return;
            }

            s = s->parent_;
        } while ((transition.target_ == nullptr) && (s != nullptr));

        if ((transition.target_ != nullptr))
        {
            if (transition.target_ != &kNone)
            {
                const auto* common_parent = FindCommonParent(current_state_, transition.target_);

                const auto* old_state = current_state_;
                current_state_ = &kInTransition;

                if ((on_state_change_ != nullptr) && (old_state != transition.target_))
                {
                    on_state_change_(*this, event, *old_state, *transition.target_);
                }

                ExitStatesFromUpTo(old_state, common_parent, event);
                transition.ExecuteActions(impl_, event);
                EnterStatesFromDownTo(common_parent, transition.target_, event);
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
     */
    void RecallEvents()
    {
        AssertionProviderType::Assert(on_recall_deferred_events_ != nullptr);
        on_recall_deferred_events_(*current_state_);
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
     * @return const char* Statemachine name
     */
    [[nodiscard]] const char* Name() const
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
        return Transition();
    }

    /**
     * @brief Defer event until state is exited
     *
     * @return Transition
     */
    static Transition DeferEvent()
    {
        return Transition(kDeferEvent);
    }

    /**
     * @brief Event was handled, but no transition shall be executed, with
     * optional action
     *
     * @return Transition
     */
    static Transition NoTransition()
    {
        return Transition(kNone);
    }
    /**
     * @brief Event was handled, but no transition shall be executed, with action
     *
     * @param action Action to execute
     * @return Transition
     */
    static Transition NoTransition(ActionType action)
    {
        return Transition(kNone, action);
    }
    /**
     * @brief Event was handled, but no transition shall be executed, with actions
     *
     * @param actions Actions to execute on transition
     * @return Transition
     */
    static Transition NoTransition(std::span<const ActionType> actions)
    {
        return Transition(kNone, actions);
    }

    /**
     * @brief Create transition to target state, with optional action
     *
     * @param target Target state
     * @return Transition
     */
    static Transition TransitionTo(StateRef target)
    {
        return Transition(target);
    }
    /**
     * @brief Create transition to target state, with action
     *
     * @param target Target state
     * @param action Action to execute on transition
     * @return Transition
     */
    static Transition TransitionTo(StateRef target, ActionType action)
    {
        return Transition(target, action);
    }
    /**
     * @brief Create transition to target state, with multiple actions
     *
     * @param target Target state
     * @param actions Actions to execute on transition
     * @return Transition
     */
    static Transition TransitionTo(StateRef target, std::span<const ActionType> actions) noexcept
    {
        return Transition(target, actions);
    }

    /**
     * @brief Stream operator for logging
     */
    friend std::ostream& operator<<(std::ostream& os, const Statemachine& sm)
    {
        return os << sm.Name();
    }

    static StatePtr FindCommonParent(StatePtr state1, StatePtr state2)
    {
        auto* state = state1;

        while (state != nullptr)
        {
            auto* parent = state2;

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

private:
    StatePtr current_state_ = nullptr;
    bool working_ = false;
    ImplPtr impl_ = nullptr;
    const char* name_ = nullptr;
    std::pmr::map<StatePtr, StatePtr> initial_;

    static const State kInTransition;
    static const State kNone;
    static const State kDeferEvent;

    void SetInitialState(StatePtr state, StatePtr initial)
    {
        if ((state->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            initial_[state] = initial;
        }
        else
        {
            // ignore
        }
    }

    StatePtr GetInitialState(StatePtr state) const
    {
        if ((state->flags_ & EFlags::kHistory) != EFlags::kNone)
        {
            auto search = initial_.find(state);
            if (search != initial_.end())
            {
                return search->second;
            }
        }

        return state->initial_;
    }

    void ExitState(StatePtr state, Event event)
    {
        if (on_state_exit_ != nullptr)
        {
            on_state_exit_(*this, *state);
        }

        for (const auto& on_ex : state->on_exit_)
        {
            (impl_->*on_ex)(event);
        }
    }

    void ExitStatesFromUpTo(StatePtr from, StatePtr top, Event event)
    {
        const auto* state = from;

        if (state == top)
        {
            ExitState(state, event);
        }
        else
        {
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

                ExitState(state, event);

                state = state->parent_;
            }
        }
    }

    void EnterState(StateRef state, Event event) const
    {
        if (on_state_entry_ != nullptr)
        {
            on_state_entry_(*this, state);
        }

        for (const auto& on_en : state.on_entry_)
        {
            (impl_->*on_en)(event);
        }
    }

    void EnterStatesFromDownToRecursive(StatePtr top, StatePtr target, Event event)
    {
        // Don't enter topmost state
        if ((target != nullptr) && (top != target))
        {
            EnterStatesFromDownToRecursive(top, target->parent_, event);
            EnterState(*target, event);
        }
    }

    void EnterStatesFromDownTo(StatePtr top, StatePtr target, Event event)
    {
        // Enter all states up to parent
        if (top != target)
        {
            EnterStatesFromDownToRecursive(top, target->parent_, event);
        }

        // Alywas enter target state (we may have exited it, possibly a self transition)
        EnterState(*target, event);

        // Is target a hierarchical state? If so, enter initial state
        const auto* state = GetInitialState(target);
        while (state != nullptr)
        {
            target = state;
            EnterState(*state, event);
            state = GetInitialState(target);
        }

        // We have reached the target state
        current_state_ = target;
    }
};

template <typename Impl, typename Event, PolymorphicAllocatorProvider Allocator,
          AssertionProvider AssertionProviderType>
const typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State
    Statemachine<Impl, Event, Allocator, AssertionProviderType>::kNone =
        typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State("None", nullptr);
template <typename Impl, typename Event, PolymorphicAllocatorProvider Allocator,
          AssertionProvider AssertionProviderType>
const typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State
    Statemachine<Impl, Event, Allocator, AssertionProviderType>::kInTransition =
        typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State("InTransition", nullptr);
template <typename Impl, typename Event, PolymorphicAllocatorProvider Allocator,
          AssertionProvider AssertionProviderType>
const typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State
    Statemachine<Impl, Event, Allocator, AssertionProviderType>::kDeferEvent =
        typename Statemachine<Impl, Event, Allocator, AssertionProviderType>::State("Defer", nullptr);
} // namespace cpp_event_framework

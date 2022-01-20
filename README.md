# cpp_event_framework

Header-only C++ event and statemachine framework

## Overview
### Events

- Shared pointers for easy handling
- Allocatable from heap or pools
- Simple to declare
- Events have IDs that can be used in switch/case
- Possibility to add data members to signal class
- Events have names for logging

### Statemachine

- Hierarchical state support (if a state does not handle an event, it is passed to parent state)
- Entry/Exit funtions
- Transition actions
- History support
- Unhandled event support
- Possibility to use same handler/entry/exit function for multiple states (state is passed as argument to functions)
- Independent of event type (can be int, enum, pointer, shared pointer...)
- Designed to be aggregated by a class
- Designed to call member functions of class aggregating statemachine
- State and transition declaration is read-only (const)
- Logging support (state entry/exit/handler events)
- States have names for logging

## Introduction

tbd

## Future ideas

- Statemachine generator e.g. from scxml or xmi

## License

Apache-2.0

## Author

Dirk Ziegelmeier <dirk@ziegelmeier.net>

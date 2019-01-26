#pragma once

#include <memory>
#include <stack>

#include "State.h"

namespace Marius
{
	typedef std::unique_ptr<State> StateRef;

	class StateMachine
	{
	public:
		StateMachine() { }
		~StateMachine() { }

		void AddState(StateRef newState, bool isReplacing = true);
		void RemoveState();
		// Run at start of each loop in Game.cpp
		void ProcessStateChanges();
		void setPlayerTurn();
		bool getPlayerTurn(bool turn);

		StateRef &GetActiveState();

	private:
		std::stack<StateRef> _states;
		StateRef _newState;

		bool _isRemoving;
		bool _isAdding, _isReplacing;

		bool _playerTurn;
	};
}
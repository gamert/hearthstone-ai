#pragma once

#include "FlowControl/FlowContext.h"

namespace mcts
{
	class ActionType
	{
	public:
		enum Types {
			kInvalid,
			kMainAction, // play card, attack, hero-power, or end-turn
			kRandom, // choose by random
			kChooseHandCard,
			kChooseAttacker,
			kChooseDefender,
			kChooseMinionPutLocation,
			kChooseTarget, // battlecry target, spell target, etc.
			kChooseOne, // choose one card id. E.g., druid choose one, tracking, adapt
		};

	public:
		ActionType() : type_(kInvalid) {}
		ActionType(Types type) : type_(type) {}

		Types GetType() const { return type_; }

		bool operator==(ActionType const& rhs) const {
			return type_ == rhs.type_;
		}
		bool operator!=(ActionType const& rhs) const { return !(*this == rhs); }

		bool IsValid() const { return type_ != kInvalid; }

		bool IsChosenRandomly() const { return type_ == kRandom; }
		bool IsChosenManually() const { return !IsChosenRandomly(); }

		bool IsInvalidStateBlameNode() const {
			// if a sub-action results in an invalid state, which sub-action should be blamed?
			switch (GetType()) {
			case ActionType::kMainAction:
				return true; // maybe no valid attacker

			case ActionType::kChooseHandCard:
				return true; // map to FlowController.PlayCard

			case ActionType::kChooseAttacker:
				return true; // map to FlowController.Attack

			case ActionType::kRandom:
			case ActionType::kChooseDefender:
			case ActionType::kChooseMinionPutLocation:
			case ActionType::kChooseTarget:
			case ActionType::kChooseOne:
				return false; // a callback sub-action
			}
			assert(false);
			return false;
		}

	private:
		Types type_;
	};

	using Result = FlowControl::Result;

	enum Stage {
		kStageSelection,
		kStageSimulation
	};
}
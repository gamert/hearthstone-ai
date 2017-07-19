#pragma once

#include "MCTS/Types.h"

namespace mcts
{
	namespace board
	{
		class ActionParameterGetter;
		class RandomGenerator;

		class BoardActionAnalyzer
		{
		private:
			enum MainActions
			{
				kActionPlayCard,
				kActionAttack,
				kActionHeroPower,
				kActionEndTurn,
				kActionMax // Should be at last
			};

		public:
			BoardActionAnalyzer() : op_map_(), op_map_size_(0), attackers_(), playable_cards_() {}

			int GetActionsCount(state::State const& board);
			Result ApplyAction(
				state::State & board,
				int action,
				RandomGenerator & random,
				ActionParameterGetter & action_parameters);

		private:
			typedef Result (BoardActionAnalyzer::*OpFunc)(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result PlayCard(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result Attack(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result HeroPower(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result EndTurn(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);

		private:
			std::array<OpFunc, kActionMax> op_map_;
			size_t op_map_size_;
			std::vector<int> attackers_;
			std::vector<size_t> playable_cards_;
		};
	}
}
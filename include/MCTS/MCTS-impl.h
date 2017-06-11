#pragma once

#include <cstdlib>

#include "MCTS/MCTS.h"
#include "MCTS/Board-impl.h"

#include "MCTS/ActionParameterGetter.h"
#include "MCTS/ActionParameterGetter.h"

namespace mcts
{
	template <typename StartBoardGetter>
	inline void MCTS::Iterate(StartBoardGetter&& start_board_getter) {
		flag_switch_to_simulation_ = false;
		episode_state_.Start(start_board_getter());
		selection_stage_.StartEpisode();
		simulation_stage_.StartEpisode();

		ActionParameterGetter action_parameter_getter(*this);
		RandomGenerator random_generator(*this);

		Result result = Result::kResultInvalid;

		while (true)
		{
			if (flag_switch_to_simulation_) {
				episode_state_.SetToSimulationStage();
				flag_switch_to_simulation_ = false;
			}

			if (episode_state_.GetStage() == detail::EpisodeState::kStageSimulation) {
				simulation_stage_.StartNewAction(episode_state_.GetBoard());
			}

			while (true)
			{
				int choices = episode_state_.GetBoard().GetActionsCount();
				int choice = this->UserChooseAction(choices);

				if (episode_state_.IsValid()) {
					result = episode_state_.GetBoard().ApplyAction(choice, random_generator, action_parameter_getter);
				}
				else {
					result = Result::kResultInvalid;
				}

				if (result == Result::kResultInvalid) {
					if (episode_state_.GetStage() == detail::EpisodeState::kStageSelection) {
						selection_stage_.ReportInvalidAction();
						return;
					}

					assert(episode_state_.GetStage() == detail::EpisodeState::kStageSimulation);

					simulation_stage_.ReportInvalidAction();
					Board const& saved_board = simulation_stage_.RestartAction();
					episode_state_.SetBoard(saved_board);
					statistic_.ApplyActionFailed();
					continue;
				}

				if (episode_state_.GetStage() == detail::EpisodeState::kStageSimulation) {
					statistic_.ApplyActionSucceeded();
				}

				break;
			}

			if (result != Result::kResultNotDetermined) break;
		}

		assert(result != Result::kResultInvalid);
		bool win = (result == Result::kResultFirstPlayerWin);

		std::for_each(
			selection_stage_.GetTraversedPath().begin(),
			selection_stage_.GetTraversedPath().end(),
			[&](TreeNode* traversed_node)
		{
			traversed_node->ReportResult(win);
		});

		statistic_.FinishedOneEpisode();
	}

	// TODO: add action type for simulation
	inline int MCTS::UserChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, false);
	}

	// TODO: unify with UserChooseAction(), distinguishing by action type
	inline int MCTS::RandomChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, true);
	}

	inline int MCTS::ActionCallback(int choices, bool random)
	{
		assert(choices > 0);

		auto stage = episode_state_.GetStage();

		if (stage == detail::EpisodeState::kStageSelection) {
			// if a new node is created, we switch to simulation
			bool & created_new_node = flag_switch_to_simulation_;

			int choice = selection_stage_.GetAction(choices, random, &created_new_node);
			if (choice < 0) episode_state_.SetInvalid();
			return choice;
		}

		if (stage == detail::EpisodeState::kStageSimulation) {
			return simulation_stage_.GetAction(choices, random);
		}

		assert(false);
		return 0;
	}
}
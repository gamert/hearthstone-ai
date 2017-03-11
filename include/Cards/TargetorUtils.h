#pragma once

#include <assert.h>
#include <vector>
#include <unordered_set>
#include <state/Types.h>

namespace Cards
{
	class TargetorInfo
	{
	public:
		TargetorInfo() :
			include_first(true), include_second(true),
			include_hero(true), include_minion(true),
			minion_filter(kMinionFilterAll)
		{
		}

		template <typename Container>
		void FillTargets(state::State const& state, Container& targets) const;

		template <>
		void FillTargets(state::State const& state, std::vector<state::CardRef>& targets) const
		{
			Process(state, [&](state::CardRef ref) {
				targets.push_back(ref);
			});
		}
		template <>
		void FillTargets(state::State const& state, std::unordered_set<state::CardRef>& targets) const
		{
			Process(state, [&](state::CardRef ref) {
				targets.insert(ref);
			});
		}

		template <typename Functor>
		void ForEach(state::State & state, state::FlowContext & flow_context, Functor&& func) const
		{
			Process(state, [&](state::CardRef ref) {
				func(state, flow_context, ref);
			});
		}

		void Count(state::State const& state, int * count) const
		{
			Process(state, [count](state::CardRef ref) {
				++(*count);
			});
		}

	private:
		template <typename Functor>
		void Process(state::State const& state, Functor&& functor) const
		{
			if (include_first) ProcessPlayerTargets(state, state.board.GetFirst(), std::forward<Functor>(functor));
			if (include_second) ProcessPlayerTargets(state, state.board.GetSecond(), std::forward<Functor>(functor));
		}

		template <typename Functor>
		void ProcessPlayerTargets(state::State const& state, state::board::Player const& player, Functor&& functor) const
		{
			if (include_hero) {
				if (player.hero_ref_ != exclude) {
					functor(player.hero_ref_);
				}
			}
			if (include_minion) {
				for (state::CardRef minion : player.minions_.Get()) {
					ProcessMinionTargets(state, minion, std::forward<Functor>(functor));
				}
			}
		}

		template <typename Functor>
		void ProcessMinionTargets(state::State const& state, state::CardRef const& minion, Functor&& functor) const
		{
			if (minion == exclude) return;

			auto const& card = state.mgr.Get(minion);

			switch (minion_filter) {
			case kMinionFilterAll:
				break;
			case kMinionFilterTargetable:
				// TODO: check stealth
				// check owning player; stealth cannot be targeted by enemy
				break;
			case kMinionFilterTargetableBySpell:
				// TODO: check stealh
				// TODO: check immune spell
				break;
			case kMinionFilterMurloc:
				if (card.GetRace() == state::kCardRaceMurloc) break;
				return;
			}

			functor(minion);
		}

	public:
		bool include_first;
		bool include_second;

		bool include_minion;
		bool include_hero;

		enum MinionFilterType
		{
			kMinionFilterAll,
			kMinionFilterTargetable,
			kMinionFilterTargetableBySpell,
			kMinionFilterMurloc
		} minion_filter;

		state::CardRef exclude;
	};

	class TargetorHelper
	{
	public: // Fluent-like API to set up
		TargetorHelper & Targetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetable;
			return *this;
		}

		TargetorHelper & SpellTargetable()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterTargetableBySpell;
			return *this;
		}

		TargetorHelper & Murlocs()
		{
			info_.minion_filter = TargetorInfo::kMinionFilterMurloc;
			return *this;
		}

		template <typename Context>
		TargetorHelper & Ally(Context&& context)
		{
			return Player(context.card_.GetPlayerIdentifier());
		}

		template <typename Context>
		TargetorHelper & Enemy(Context&& context)
		{
			return AnotherPlayer(context.card_.GetPlayerIdentifier());
		}

		TargetorHelper & Minion()
		{
			info_.include_hero = false;
			assert(info_.include_minion == true);
			return *this;
		}

		TargetorHelper & Hero()
		{
			info_.include_minion = false;
			assert(info_.include_hero == true);
			return *this;
		}

		TargetorHelper & Exclude(state::CardRef ref)
		{
			assert(info_.exclude.IsValid() == false); // only support one exclusion
			info_.exclude = ref;
			return *this;
		}

	private:
		TargetorHelper & Player(state::PlayerIdentifier player)
		{
			if (player.IsFirst()) {
				info_.include_first = true;
				info_.include_second = false;
			}
			else {
				info_.include_first = false;
				info_.include_second = true;
			}
			return *this;
		}

		TargetorHelper & AnotherPlayer(state::PlayerIdentifier player)
		{
			return Player(player.Opposite());
		}

	public:
		TargetorInfo GetInfo() const { return info_; }

	private:
		TargetorInfo info_;
	};
}
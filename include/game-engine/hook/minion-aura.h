#pragma once

#include <stdexcept>

#include "game-engine/hook/listener.h"
#include "game-engine/enchantments/owner.h"
#include "game-engine/player.h"

namespace GameEngine
{
	class Minion;

	// An aura (i.e., hook listener) hold by a minion
	class MinionAura : public HookListener
	{
		friend std::hash<MinionAura>;

	public:
		MinionAura(Minion & owner) : owner(owner) {}
		~MinionAura() {}

		Minion & GetOwner() const { return this->owner; }

	public:
		void BeforeRemoved(Minion & owner)
		{
			HookListener::BeforeRemoved(owner);

			if (this->minion_enchantments) {
				this->minion_enchantments->RemoveOwnedEnchantments();
			}
			if (this->player_enchantments) {
				this->player_enchantments->RemoveOwnedEnchantments();
			}
		}

	public:
		template <typename Target> void GetEnchantmentsOwner(EnchantmentOwner<Target> * &owner);
		template <> void GetEnchantmentsOwner(EnchantmentOwner<Minion> * &owner) { owner = &this->GetMinionEnchantmentsOwner(); }
		template <> void GetEnchantmentsOwner(EnchantmentOwner<Player> * &owner) { owner = &this->GetPlayerEnchantmentsOwner(); }

		EnchantmentOwner<Minion> & GetMinionEnchantmentsOwner()
		{
			if (!this->minion_enchantments) {
				this->minion_enchantments.reset(new EnchantmentOwner<Minion>());
			}
			return *this->minion_enchantments;
		}

		EnchantmentOwner<Player> & GetPlayerEnchantmentsOwner()
		{
			if (!this->player_enchantments) {
				this->player_enchantments.reset(new EnchantmentOwner<Player>());
			}
			return *this->player_enchantments;
		}

	private:
		Minion & owner;
		std::unique_ptr<EnchantmentOwner<Minion>> minion_enchantments;
		std::unique_ptr<EnchantmentOwner<Player>> player_enchantments;
	};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::MinionAura> {
		typedef GameEngine::MinionAura argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return s.GetHash();
		}
	};
}
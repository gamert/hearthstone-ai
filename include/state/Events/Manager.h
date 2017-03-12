#pragma once

#include <type_traits>
#include <utility>
#include "state/Types.h"
#include "state/Events/impl/HandlersContainer.h"
#include "state/Events/impl/CategorizedHandlersContainer.h"

#include "state/Events/EventTypes/AfterMinionSummoned.h"
#include "state/Events/EventTypes/BeforeMinionSummoned.h"
#include "state/Events/EventTypes/AfterMinionPlayed.h"
#include "state/Events/EventTypes/OnMinionPlay.h"
#include "state/Events/EventTypes/OnTurnEnd.h"
#include "state/Events/EventTypes/OnTurnStart.h"
#include "state/Events/EventTypes/BeforeAttack.h"
#include "state/Events/EventTypes/OnAttack.h"
#include "state/Events/EventTypes/AfterAttack.h"
#include "state/Events/EventTypes/OnTakeDamage.h"
#include "state/Events/EventTypes/OnHeal.h"
#include "state/Events/EventTypes/UpdateAura.h"

namespace state
{
	namespace Events
	{
		template <typename T> class Event;
		template <typename T> class CategorizedEvent;

		class Manager
		{
			template <typename T> friend class Event;
			template <typename T> friend class CategorizedEvent;

		public:
			template <typename EventType, typename T>
			void PushBack(T&& handler) {
				GetHandlersContainer<EventType>().PushBack(std::forward<T>(handler));
			}

			template <typename EventType, typename T>
			void PushBack(CardRef card_ref, T&& handler) {
				GetCategorizedHandlersContainer<EventType>().PushBack(
					card_ref, std::forward<T>(handler));
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEvent(Args&&... args)
			{
				return GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEvent(CardRef card_ref, Args&&... args)
			{
				return GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAll(card_ref, std::forward<Args>(args)...);
			}

		private:
			template<typename EventHandlerType>
			impl::HandlersContainer<EventHandlerType> & GetHandlersContainer();

			template<typename EventHandlerType>
			impl::CategorizedHandlersContainer<EventHandlerType> & GetCategorizedHandlersContainer();

#define ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::HandlersContainer<EventTypes::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_TRIGGER_TYPE(TYPE_NAME) ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> & GetCategorizedHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_CATEGORIZED_TRIGGER_TYPE(TYPE_NAME) ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)

			ADD_TRIGGER_TYPE(AfterMinionSummoned);
			ADD_TRIGGER_TYPE(BeforeMinionSummoned);
			ADD_TRIGGER_TYPE(AfterMinionPlayed);
			ADD_TRIGGER_TYPE(OnMinionPlay);
			ADD_TRIGGER_TYPE(OnTurnEnd);
			ADD_TRIGGER_TYPE(OnTurnStart);

			ADD_TRIGGER_TYPE(BeforeAttack);
			ADD_CATEGORIZED_TRIGGER_TYPE(BeforeAttack);

			ADD_TRIGGER_TYPE(OnAttack);
			ADD_CATEGORIZED_TRIGGER_TYPE(OnAttack);

			ADD_TRIGGER_TYPE(AfterAttack);
			ADD_CATEGORIZED_TRIGGER_TYPE(AfterAttack);

			ADD_TRIGGER_TYPE(OnTakeDamage);
			ADD_CATEGORIZED_TRIGGER_TYPE(OnTakeDamage);

			ADD_TRIGGER_TYPE(OnHeal);
			ADD_CATEGORIZED_TRIGGER_TYPE(OnHeal);

			ADD_TRIGGER_TYPE(UpdateAura);

#undef ADD_TRIGGER_TYPE_INTERNAL
#undef ADD_TRIGGER_TYPE
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL
#undef ADD_CATEGORIZED_TRIGGER_TYPE
		};

	}
}
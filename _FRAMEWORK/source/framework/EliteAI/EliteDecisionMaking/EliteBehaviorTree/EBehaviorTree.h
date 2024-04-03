#ifndef ELITE_BEHAVIOR_TREE
#define ELITE_BEHAVIOR_TREE

#include "framework/EliteAI/EliteData/EBlackboard.h"

namespace Elite
{
	enum class BehaviorState
	{
		Failure,
		Success,
		Running
	};

	class IBehavior
	{
		public:
			IBehavior();
			virtual ~IBehavior() = default;

			IBehavior(const IBehavior&) = delete;
			IBehavior& operator=(const IBehavior&) = delete;
			IBehavior(IBehavior&&) = delete;
			IBehavior& operator=(IBehavior&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) = 0;

		protected:
			BehaviorState m_CurrentState;
	};

	class BehaviorComposite : public IBehavior
	{
		public:
			explicit BehaviorComposite(std::vector<IBehavior*> childBehaviors);
			virtual ~BehaviorComposite();

			BehaviorComposite(const BehaviorComposite&) = delete;
			BehaviorComposite& operator=(const BehaviorComposite&) = delete;
			BehaviorComposite(BehaviorComposite&&) = delete;
			BehaviorComposite& operator=(BehaviorComposite&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override = 0;

		protected:
			std::vector<IBehavior*> m_ChildBehaviors;
	};

	class BehaviorSelector final : public BehaviorComposite
	{
		public:
			explicit BehaviorSelector(std::vector<IBehavior*> childBehaviors);
			virtual ~BehaviorSelector() = default;

			BehaviorSelector(const BehaviorSelector&) = delete;
			BehaviorSelector& operator=(const BehaviorSelector&) = delete;
			BehaviorSelector(BehaviorSelector&&) = delete;
			BehaviorSelector& operator=(BehaviorSelector&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override;
	};

	class BehaviorSequence : public BehaviorComposite
	{
		public:
			explicit BehaviorSequence(std::vector<IBehavior*> childBehaviors);
			virtual ~BehaviorSequence() = default;

			BehaviorSequence(const BehaviorSequence&) = delete;
			BehaviorSequence& operator=(const BehaviorSequence&) = delete;
			BehaviorSequence(BehaviorSequence&&) = delete;
			BehaviorSequence& operator=(BehaviorSequence&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override;
	};

	class BehaviorPartialSequence final : public BehaviorSequence
	{
		public:
			explicit BehaviorPartialSequence(std::vector<IBehavior*> childBehaviors);
			virtual ~BehaviorPartialSequence() = default;

			BehaviorPartialSequence(const BehaviorPartialSequence&) = delete;
			BehaviorPartialSequence& operator=(const BehaviorPartialSequence&) = delete;
			BehaviorPartialSequence(BehaviorPartialSequence&&) = delete;
			BehaviorPartialSequence& operator=(BehaviorPartialSequence&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override;

		private:
			unsigned int m_CurrentBehaviorIndex;
	};

	class BehaviorConditional final : public IBehavior
	{
		public:
			explicit BehaviorConditional(std::function<bool(Blackboard*)> function);
			virtual ~BehaviorConditional() = default;

			BehaviorConditional(const BehaviorConditional&) = delete;
			BehaviorConditional& operator=(const BehaviorConditional&) = delete;
			BehaviorConditional(BehaviorConditional&&) = delete;
			BehaviorConditional& operator=(BehaviorConditional&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override;

		private:
			std::function<bool(Blackboard*)> m_fpConditional;
	};

	class BehaviorAction final : public IBehavior
	{
		public:
			explicit BehaviorAction(std::function<BehaviorState(Blackboard*)> function);
			virtual ~BehaviorAction() = default;

			BehaviorAction(const BehaviorAction&) = delete;
			BehaviorAction& operator=(const BehaviorAction&) = delete;
			BehaviorAction(BehaviorAction&&) = delete;
			BehaviorAction& operator=(BehaviorAction&&) = delete;

			virtual BehaviorState Execute(Blackboard* pBlackBoard) override;

		private:
			std::function<BehaviorState(Blackboard*)> m_fpAction;
	};

	class BehaviorTree final : public IDecisionMaking
	{
		public:
			explicit BehaviorTree(Blackboard* pBlackBoard, IBehavior* pRootBehavior);
			virtual ~BehaviorTree();

			BehaviorTree(const BehaviorTree&) = delete;
			BehaviorTree& operator=(const BehaviorTree&) = delete;
			BehaviorTree(BehaviorTree&&) = delete;
			BehaviorTree& operator=(BehaviorTree&&) = delete;

			virtual void Update(float deltaTime) override;
			Blackboard* GetBlackboard() const;

		private:
			BehaviorState m_CurrentState;
			Blackboard* m_pBlackBoard;
			IBehavior* m_pRootBehavior;
	};
}
#endif
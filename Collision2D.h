#pragma once
#include <vector>
#include <list>
#include <ranges>
#include "Boundaries2D.h"

namespace KEngine2D {


	typedef unsigned int ColliderHandle;

	namespace Zomorodian_Edelsbrunner {
		struct Interval
		{
			int lo;
			int hi;
			static Interval All() {
				return { std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max() };
			}
			bool empty() {
				return lo >= hi;
			}
		};


		struct Box
		{
			Interval intervals[2];
			ColliderHandle boxId;
		};

        
        bool Intersects(const Interval& interval, const Interval& other);
        bool Spans(const Interval& interval, const Interval& other);

        void BruteCheck(std::vector<std::pair<int, int>>& output, Box& box, Box& otherBox, int dimension);
        
#ifdef __cpp_lib_ranges

		template<typename T>
		concept BoxRange = std::ranges::viewable_range<T> && requires (T t) {
			{t.operator[](0)} -> std::convertible_to<Box>;
		};
    
		int ApproxMedian(BoxRange auto boxes, int dimension);
		void OneWayScan(std::vector<std::pair<int, int>>& output, BoxRange auto leftBoxes, BoxRange auto rightBoxes, int dimension);
		void ModifiedTwoWayScan(std::vector<std::pair<int, int>>& output, BoxRange auto leftBoxes, BoxRange auto rightBoxes, int dimension);
		void Hybrid3(std::vector<std::pair<int, int>> &output, BoxRange auto leftBoxes, BoxRange auto rightBoxes, Interval interval, int dimension, int cutoff);
#else
    struct BoxRange
    {
        std::vector<Box>::iterator mBegin;
        std::vector<Box>::iterator mEnd;
        
        std::vector<Box>::iterator begin() {return mBegin;}
        std::vector<Box>::iterator end() { return mEnd;}
        
        bool empty() {
            return mBegin == mEnd;
        }
        
        size_t size() {
            return std::distance(begin(), end());
        }
        
        Box& operator[](int n)
        {
            auto it = begin();
            std::advance(it, n);
            return *it;
        }
    };
    
    int ApproxMedian(BoxRange  boxes, int dimension);
    void OneWayScan(std::vector<std::pair<int, int>>& output, BoxRange  leftBoxes, BoxRange  rightBoxes, int dimension);
    void ModifiedTwoWayScan(std::vector<std::pair<int, int>>& output, BoxRange  leftBoxes, BoxRange  rightBoxes, int dimension);
    void Hybrid3(std::vector<std::pair<int, int>> &output, BoxRange  leftBoxes, BoxRange  rightBoxes, Interval interval, int dimension, int cutoff);
    
#endif
	}

	class BroadPhaseCollider
	{
	public:
		struct AABB
		{
			Point			mUpLeft;
			Point			mDownRight;
			ColliderHandle	mHandle;
		};
		std::vector<std::pair<int, int>>& GetBroadPhaseCollisions(std::vector<AABB> leftAABBs, std::vector<AABB> rightAABBs);
		std::vector<std::pair<int, int>>& GetBroadPhaseCollisions(std::vector<AABB> allAABBs);
	private:
		
		std::vector<std::pair<int, int>> mRetVal;
		std::vector<Zomorodian_Edelsbrunner::Box> mLeftBoxes;
		std::vector<Zomorodian_Edelsbrunner::Box> mRightBoxes;
	};
	
	struct Collision
	{
		ColliderHandle first;
		ColliderHandle second;
		Point		   collisionPoint;
		Point		   collisionNormal;
	};

	class CollisionDispatcher
	{
	public:
		CollisionDispatcher() {}
		virtual ~CollisionDispatcher() { Deinit(); }

		virtual void Deinit() {}
		virtual void HandleCollision(Collision colliderHandles) = 0;
	};
		
	class CollisionSystem;

	class Collider
	{
	public:
		Collider();
		~Collider();

		void Init(CollisionSystem * system, BoundingArea * bounds, unsigned int flags = 0xFFFFFFFF, unsigned int filters = 0xFFFFFFFF);
		void Deinit();
	
		bool Collidable(const Collider* other) const;
		const BoundingArea* GetBounds() const;

		ColliderHandle GetHandle();
	private:
		CollisionSystem*	mSystem{ nullptr };
		BoundingArea*		mBounds{ nullptr };
		ColliderHandle		mHandle;
		unsigned int		mFlags;
		unsigned int		mFilters;
	};

	class CollisionSystem
	{
	public:
		CollisionSystem();
		~CollisionSystem();
		void Init(CollisionDispatcher* dispatcher);
		void Deinit();

		ColliderHandle AddCollider(Collider* collider);
		void RemoveCollider(Collider* collider);

		void Update();

	private:
		unsigned int							mNextColliderHandle{ 0 };
		std::map<ColliderHandle, Collider*>		mColliders;
		CollisionDispatcher* mDispatcher		{ nullptr };
		BroadPhaseCollider						mBpc;
		std::vector<BroadPhaseCollider::AABB>	mAABBs;
	};
}


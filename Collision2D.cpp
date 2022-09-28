#include "Collision2D.h"
#include <algorithm>
#include <ranges>
#include <random>
#include <set>

std::vector<std::pair<int, int>>& KEngine2D::BroadPhaseCollider::GetBroadPhaseCollisions(std::vector<AABB> leftAABBs, std::vector<AABB> rightAABBs)
{
    mRetVal.clear();
    mLeftBoxes.clear();
    mRightBoxes.clear();

    auto convertBoxes = [](const std::vector<AABB>& aabbs, std::vector<Zomorodian_Edelsbrunner::Box>& boxes) {
        for (auto boundingBox : aabbs)
        {
            Zomorodian_Edelsbrunner::Box box;
            box.boxId = boundingBox.mHandle;
            box.intervals[0] = { (int)std::floor(boundingBox.mUpLeft.x), (int)std::ceil(boundingBox.mDownRight.x) };
            box.intervals[1] = { (int)std::floor(boundingBox.mUpLeft.y), (int)std::ceil(boundingBox.mDownRight.y) };
            boxes.push_back(box);
        }
    };

    convertBoxes(leftAABBs, mLeftBoxes);
    convertBoxes(rightAABBs, mRightBoxes);

    Zomorodian_Edelsbrunner::Hybrid3(mRetVal, mLeftBoxes, mRightBoxes, Zomorodian_Edelsbrunner::Interval::All(), 2, 1000);
    return mRetVal;
}


std::vector<std::pair<int, int>>& KEngine2D::BroadPhaseCollider::GetBroadPhaseCollisions(std::vector<AABB> allAABBs)
{
    return GetBroadPhaseCollisions(allAABBs, allAABBs);
}

bool KEngine2D::Zomorodian_Edelsbrunner::Intersects(const Interval& interval, const Interval& other)
{
    return (interval.lo < other.hi && interval.lo >= other.lo) || (other.lo < interval.hi && other.lo >= interval.lo);
}

bool KEngine2D::Zomorodian_Edelsbrunner::Spans(const Interval& interval, const Interval& other)
{
    return interval.lo <= other.lo && interval.hi > other.hi;
}

void KEngine2D::Zomorodian_Edelsbrunner::BruteCheck(std::vector<std::pair<int, int>>& output, Box& box, Box& otherBox, int dimension)
{
    bool intersects = box.boxId != otherBox.boxId;
    for (int dim = dimension; intersects && dim >= 0; dim--) {
           intersects = Intersects(box.intervals[dim], otherBox.intervals[dim]);
    }
    if (intersects)
    {
        output.push_back({ box.boxId, otherBox.boxId });
    }
}

int KEngine2D::Zomorodian_Edelsbrunner::ApproxMedian(BoxRange auto boxes, int dimension)
{
    //This is not the ApproxMedian algorithm used by Zomorodian and Edelsbrunner (2000).  It should be more accurate and just as fast.

    // Declaration for the random number generator
    static std::random_device rand_dev;
    static std::mt19937 generator(rand_dev());
    
    int n = boxes.size();
    
    // Random number generated will be in the range [0,n-1]
    std::uniform_int_distribution distribution(0, n - 1);
    
    if (n == 0)
        return 0;
    
    int k = int(10 * log2(n)); // Taking c as 10
    
    // A set stores unique elements in sorted order
    std::set<int> s;
    for (int i = 0; i < k; i++)
    {
        // Generating a random index
        int index = distribution(generator);
    
        //Inserting into the set
        s.insert(boxes[index].intervals[dimension].lo);
    }
    
    auto itr = s.begin();
    
    // Report the median of the set at k/2 position
    // Move the itr to k/2th position
    advance(itr, (s.size() / 2) - 1);
    
    // Return the median
    return *itr;
}


void KEngine2D::Zomorodian_Edelsbrunner::OneWayScan(std::vector<std::pair<int, int>>& output, BoxRange auto leftBoxes, BoxRange auto rightBoxes, int dimension)
{
    auto boxComparator = [dimension](Box& box, Box& otherBox) {
        return box.intervals[dimension].lo < otherBox.intervals[dimension].lo;
    };

    std::ranges::sort(leftBoxes, boxComparator);
    std::ranges::sort(rightBoxes, boxComparator);

    auto pIter = rightBoxes.begin();
    for (Box& intervalBox : leftBoxes)
    {
        while (pIter != rightBoxes.end() && pIter->intervals[dimension].lo < intervalBox.intervals[dimension].lo)
        {
            pIter++;
        }
        for (auto pIter2 = pIter; pIter2 != rightBoxes.end() && pIter2->intervals[dimension].lo < intervalBox.intervals[dimension].hi; pIter2++) {
            BruteCheck(output, intervalBox, *pIter2, dimension);
        }
    }
}

void KEngine2D::Zomorodian_Edelsbrunner::ModifiedTwoWayScan(std::vector<std::pair<int, int>>& output, BoxRange auto leftBoxes, BoxRange auto rightBoxes, int dimension)
{
    auto boxComparator = [dimension](Box& box, Box& otherBox) {
        return box.intervals[dimension].lo < otherBox.intervals[dimension].lo;
    };

    std::ranges::sort(leftBoxes, boxComparator);
    std::ranges::sort(rightBoxes, boxComparator);

    auto lIter = leftBoxes.begin();
    auto rIter = rightBoxes.begin();

    while (lIter != leftBoxes.end() && rIter != rightBoxes.end()) {
        if (lIter->intervals[dimension].lo < rIter->intervals[dimension].lo)
        {
            for (auto pIter = rIter; pIter != rightBoxes.end() && pIter->intervals[dimension].lo < lIter->intervals[dimension].hi; pIter++)
            {
                BruteCheck(output, *lIter, *pIter, dimension);
            }
            lIter++;
        }
        else {
            for (auto pIter = lIter; pIter != leftBoxes.end() && pIter->intervals[dimension].lo < rIter->intervals[dimension].hi; pIter++)
            {
                BruteCheck(output, *pIter, *rIter, dimension);
            }
            rIter++;
        }
    }
}


void KEngine2D::Zomorodian_Edelsbrunner::Hybrid3(std::vector<std::pair<int, int>>& output, BoxRange auto leftBoxIntervals, BoxRange auto rightBoxPoints, Interval interval, int dimension, int cutoff)
{
    if (leftBoxIntervals.empty() || rightBoxPoints.empty() || interval.empty())
    {
        return;
    }

    if (dimension == 0)
    {
        OneWayScan(output, leftBoxIntervals, rightBoxPoints, 0);
        return;
    }

    if (leftBoxIntervals.size() < cutoff || rightBoxPoints.size() < cutoff)
    {
        ModifiedTwoWayScan(output, leftBoxIntervals, rightBoxPoints, dimension);
        return;
    }

    std::vector<std::pair<int, int>> retVal;


    int lowIndex = 0;
    int highIndex = leftBoxIntervals.size() - 1;

    auto nonSpanIntervalsRange = std::ranges::partition(leftBoxIntervals, [&](const auto& box) {
        return Spans(box.intervals[dimension], interval);
        });

    auto spanIntervalsRange = std::ranges::subrange(leftBoxIntervals.begin(), nonSpanIntervalsRange.begin());

    Hybrid3(output, spanIntervalsRange, rightBoxPoints, Interval::All(), dimension - 1, cutoff);
    Hybrid3(output, rightBoxPoints, spanIntervalsRange, Interval::All(), dimension - 1, cutoff);


    int split = ApproxMedian(rightBoxPoints, dimension);
    auto rightPointRange = std::ranges::partition(rightBoxPoints, [&](const auto& box) {
        return box.intervals[dimension].lo >= split;
    });
    auto leftPointRange = std::ranges::subrange(rightBoxPoints.begin(), rightPointRange.begin());

    Interval leftInterval = { interval.lo, split };

    auto leftIntervalRange = std::ranges::partition(nonSpanIntervalsRange, [&](const auto& box) {
        return Intersects(box.intervals[dimension], leftInterval);
    });

    Hybrid3(output, leftIntervalRange, leftPointRange, leftInterval, dimension, cutoff);

    Interval rightInterval = { split, interval.hi };
    auto rightIntervalRange = std::ranges::partition(nonSpanIntervalsRange, [&](const auto& box) {
        return Intersects(box.intervals[dimension], rightInterval);
    });

    Hybrid3(output, rightIntervalRange, rightPointRange, rightInterval, dimension, cutoff);
}

KEngine2D::Collider::Collider()
{
}

KEngine2D::Collider::~Collider()
{
    Deinit();
}

void KEngine2D::Collider::Init(CollisionSystem* system, BoundingArea* bounds, unsigned int flags, unsigned int filters)
{
    assert(mSystem == nullptr);
    mBounds = bounds;
    mFlags = flags;
    mFilters = filters;
    mHandle = system->AddCollider(this);
    mSystem = system;
}

void KEngine2D::Collider::Deinit()
{
    if (mSystem != nullptr)
    {
        mSystem->RemoveCollider(this);
    }
    mSystem = nullptr;
}

bool KEngine2D::Collider::Collidable(const Collider* other) const
{
    return mFilters & other->mFlags;
}

const KEngine2D::BoundingArea* KEngine2D::Collider::GetBounds() const
{
    return mBounds;
}

KEngine2D::ColliderHandle KEngine2D::Collider::GetHandle()
{
    return mHandle;
}

KEngine2D::CollisionSystem::CollisionSystem()
{
}

KEngine2D::CollisionSystem::~CollisionSystem()
{
    Deinit();
}

void KEngine2D::CollisionSystem::Init(CollisionDispatcher * dispatcher)
{
    assert(mDispatcher == nullptr);
    mDispatcher = dispatcher;
}

void KEngine2D::CollisionSystem::Deinit()
{
    assert(mColliders.empty());
    mDispatcher = nullptr;
}

KEngine2D::ColliderHandle KEngine2D::CollisionSystem::AddCollider(Collider* collider)
{
    ColliderHandle handle = mNextColliderHandle++;
    assert(!mColliders.contains(handle)); //Oh wow, how did you do that.
    mColliders[handle] = collider;
    return handle;
}

void KEngine2D::CollisionSystem::RemoveCollider(Collider* collider)
{
    mColliders.erase(collider->GetHandle());
}

void KEngine2D::CollisionSystem::Update()
{
    mAABBs.clear();
    for (auto colliderPair : mColliders)
    {
        auto collider = colliderPair.second;

        auto aabbPair = collider->GetBounds()->GetAxisAlignedBoundingBox();
        mAABBs.push_back({ aabbPair.first, aabbPair.second, collider->GetHandle() });
    }


    std::vector<std::pair<int, int>> broadPhaseCollisionList = mBpc.GetBroadPhaseCollisions(mAABBs);

    
    for (auto pair : broadPhaseCollisionList)
    {
        auto * left = mColliders[pair.first];
        auto * right = mColliders[pair.second];

        if (left->Collidable(right)) 
        {
            auto collisionInfo = left->GetBounds()->Collides(*right->GetBounds());
            if (collisionInfo.collides)
            {
                mDispatcher->HandleCollision({ left->GetHandle(), right->GetHandle(), collisionInfo.collisionPoint, collisionInfo.collisionNormal });
            }
        }
        else if (right->Collidable(left))
        {
            auto collisionInfo = right->GetBounds()->Collides(*left->GetBounds());
            if (collisionInfo.collides)
            {
                mDispatcher->HandleCollision({ right->GetHandle(), left->GetHandle(), collisionInfo.collisionPoint, collisionInfo.collisionNormal });
            }
        }
    }
}


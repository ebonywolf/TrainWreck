#pragma once

using InnerConnType =double;

class Stop;
using Stopptr = std::shared_ptr<Stop>;

class Stop : public Entity<StopData,Void>,public Entity<bool,Void>, public Entity< Stopptr,Void>, public Entity<InnerConnType,InnerConnType>,public Common<Stop>
{
    using StopInfoConn =  Entity<StopData,Void>;
    using RouteInfoConn =  Entity<Stopptr,Void>;
    using InnerConn = Entity<InnerConnType,InnerConnType>;

    StopData data;
public:
    Stop()
    {
        StopInfoConn::setRunFunction(init);
        RouteInfoConn::setRunFunction(routes);
    }
    static void init(StopInfoConn::Entityptr me)
    {
        Stopptr root = getRoot(me);
        auto changed = me->getChanged();
        StopData val = changed->getValue(me);
        root->data =val;
    }
    static void routes(RouteInfoConn::Entityptr me)
    {
        Stopptr root = getRoot(me);
        auto changed = me->getChanged();
        auto other = changed->getValue(me);

        InnerConnType distance = root->distance(other);
        root->InnerConn::receiveData(other, distance);
    }
    double distance(Stopptr other)
    {
        return data.position.distance(other->data.position);
    }

};

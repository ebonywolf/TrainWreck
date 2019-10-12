#pragma once

using InnerConnType =double;

class Stop;
using Stopptr = std::shared_ptr<Stop>;

class Stop : public Entity<StopData,Void>,public Entity<bool,Void>, public Entity< Stopptr,Void>,
 public Entity<InnerConnType,InnerConnType>,
 public Common<Stop>
{
    using stopInfoConn =  Entity<StopData,Void>;
    using routeInfoConn =  Entity<Stopptr,Void>;
    using innerConn = Entity<InnerConnType,InnerConnType>;
    using searchConn = Entity<bool,Void>;


    bool end=0;
public:
      StopData data;
    Stop()
    {
        stopInfoConn::setRunFunction(init);
        routeInfoConn::setRunFunction(routes);
        searchConn::setRunFunction(startSearch);
        innerConn::setMemoryFunction(memory);
    }
    static void startSearch(searchConn::Entityptr me){

        auto root = getRoot(me);
        auto changed = me->getChanged();
        bool val = changed->getValue(me);

        if(val==START){
            root->propagate(0);
            std::cerr<<root->data<<"Propagating"<<std::endl;
        }else{
            root->end = true;
        }
    }

    static void memory(innerConn::Entityptr me, innerConn::Negaptr notme, InnerConnType data)
    {
        auto root = getRoot(me);
        auto notme_root = getRoot(notme);
        root->connections[notme_root]-=data;

        std::cerr<< "Message:"<<notme_root->data<<"->"<<data<<" ->"<<root->data<<std::endl;

    }


    static void init(stopInfoConn::Entityptr me)
    {
        Stopptr root = getRoot(me);
        auto changed = me->getChanged();
        StopData val = changed->getValue(me);
        root->data =val;
    }
    static void routes(routeInfoConn::Entityptr me)
    {
      std::cout<<__PRETTY_FUNCTION__<<std::endl;
        Stopptr root = getRoot(me);
        auto changed = me->getChanged();
        auto other = changed->getValue(me);

        std::cerr<<"Route:"<<root->data<<" "<<other->data<<std::endl;

        root->connections[other]= root->distance(other);

    }
    void propagate(double dist){
        double smallest=999999;
        Stopptr ptr;
        for(auto& x: connections)
        {
            auto to = x.first;
            auto val = x.second;
            if(val < smallest){
                ptr = to;
            }
        }
        innerConn::sendValue(ptr,smallest );
    }

    double distance(Stopptr other)
    {
        return data.position.distance(other->data.position);
    }
    std::unordered_map<Stopptr,double> connections;


};

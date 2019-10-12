
#pragma once


struct PathFinder : public Entity<MapDataPtr,Path>, public Entity<Void,StopData>,
    public Entity<Void,Stopptr>, public Entity<Path,Strtuple>, public MultiInstance<PathFinder> {

    using mainConn = Entity<MapDataPtr,Path>;
    using nodeConn =  Entity<Void,StopData>;
    using routeConn = Entity<Void,Stopptr>;
    using gridConn = Entity<Path,Strtuple>;

    PathFinder()
    {
        mainConn::setRunFunction(CreateChart);
    }

    static void CreateChart(mainConn::Entityptr me)
    {
        auto root = getRoot(me);
        auto changed = me->getChanged();
        MapDataPtr mapa = changed->getValue(me);

        std::cerr<<"Creating:"<< mapa->routes.size()<<std::endl;

        root->createMap(mapa);

    }
    void createMap(MapDataPtr mapa)
    {
        auto grid = Grid::get();

        for(auto& x: (*mapa.get()) ) {
            StopData data = x.second;
            std::string key = x.first;
            Stopptr stop = grid->at(key);
            nodeConn::sendValue(stop, data );
        }
        for(auto& x: mapa->routes) {
            auto from = grid->at(x.first);
            auto to = grid->at(x.second);
            std::cerr<<"Set route:"<<from->data<<" "<<to->data<<std::endl;
            routeConn::sendValue(from, to);
        }

        Strtuple tup = {mapa->startPoint,mapa->endPoint};
        gridConn::sendValue(grid,tup);
    }

};

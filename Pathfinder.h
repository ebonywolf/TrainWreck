
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

    static CreateChart(mainConn::Entityptr me)
    {
        auto root = getRoot(me);
        for(auto& x:me->getChangedEntities()) {
            //should never have more than 1
            MapDataPtr mapa = x->getValue(me);
            root->createMap(mapa);
        }
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
            routeConn::sendValue(from, to);
        }
        Strtuple tup = {mapa->startPoint,mapa->endPoint};
        gridConn::sendValue(grid,tup);
    }

};

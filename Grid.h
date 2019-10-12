#pragma once



struct Grid:  public Entity<MapDataPtr,Path>,public Entity<Strtuple,Path>, public Entity<Void,bool>, public MultiInstance<Grid>, public MapInstance<std::string, Stop>
{
    using pathConn = Entity<Strtuple,Path>;
    using searchConn = Entity<Void,bool>;

    Grid()
    {
        pathConn::setRunFunction(tracePath);
    }
    static void tracePath(pathConn::Entityptr me)
    {
        auto root = getRoot(me);
        auto changed = me->getChanged();
        Strtuple val = changed->getValue(me);

        root->searchConn::sendValue( root->at(val.first), START );
        root->searchConn::sendValue( root->at(val.second), END );
    }

};


#pragma once

#include <bits/stdc++.h>
#include "Coord.h"

struct StopData {
    std::string name;
    std::string id;
    Coord position;
    friend
    std::ostream& operator<<(std::ostream& os ,const StopData& data){
        os<<data.id<<" "<<data.name;
        return os;
    }
};

struct MapData: public  std::unordered_map<std::string,StopData>{
    std::string startPoint,endPoint;
    std::unordered_map<std::string,std::string> routes;
};
using MapDataPtr = std::shared_ptr<MapData>;

struct Strtuple {
    std::string first;
    std::string second;
};

struct Path : public std::vector<std::string>{
};

#pragma once

#include "Apollo/Entity.h"
#include <bits/stdc++.h>
#include "Apollo/Specials.h"
#include "Data.h"
class StopData;
class InputParser;



struct InputParser: public pg::Entity<std::string, StopData>,  public pg::MultiInstance<InputParser> {
    using InputSrc =  Entity<std::string, StopData>;

    InputParser()
    {
        setRunFunction( InputParser::run );
    }

    static void run( Entityptr me)
    {
        for(auto& x:me->getChangedEntities()) {
            std::string val = x->getValue(me);
            me->sendValue(x, parseInput(val) );
        }
    }
    static StopData parseInput(std::string line)
    {
        StopData data;
        std::vector<std::string> result;
        std::stringstream ss(line);
        while( ss.good() ) {
            std::string substr;
            getline( ss, substr, ',' );
            result.push_back( substr );
        }
        data.id = result[0];
        data.name = result[1];
        data.position = { std::stoi(result[3]), std::stoi(result[4]) };
        return data;
    };

};

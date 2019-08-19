#include <bits/stdc++.h>
#include "Apollo/Entity.h"
#include "Apollo/Specials.h"



using namespace pg;

#include "Data.h"
#include "InputParser.h"
#include "Stop.h"
#include "Grid.h"
#include "Pathfinder.h"
using namespace std;



struct Main : public Entity<StopData, std::string>,public Entity<Path,MapDataPtr>, public pg::Singleton<Main> {
    using Parser = Entity<StopData, std::string>;
    using PathRequester = Entity<Path,MapDataPtr>;

    MapDataPtr mapData;

    std::istream& ss;
    Main():ss(std::cin)
    {
        init();
    }
    Main(std::istream& in):ss(in)
    {
        init();
    }

    void init()
    {
        Parser::setRunFunction( Main::ack );
        mapData = make_shared<MapData>();
    }
    MapData& getData()
    {
        return *mapData.get();
    }


    static ack(Parser::Entityptr me)
    {
        std::cout<<__PRETTY_FUNCTION__<<std::endl;
        auto root = getRoot(me);
        for(auto& x: me->getChangedEntities()) {
            StopData data = x->getValue(me);
            root->getData()[data.id]= data;

        }
        auto pathfinder = PathFinder::get();
        root->PathRequester::sendValue(pathfinder,  root->mapData);
    }

    void run()
    {
        string startPoint;
        ss >> startPoint;
        ss.ignore();
        std::cout<<startPoint<<std::endl;
        string endPoint;
        ss >> endPoint;
        ss.ignore();
        std::cout<<endPoint<<std::endl;
        int N;
        ss >> N;
        ss.ignore();
        for (int i = 0; i < N; i++) {
            string stopName;
            getline(ss, stopName);
            std::cout<<stopName<<std::endl;
            auto inputParser = InputParser::get();
            Parser::sendValue( inputParser,stopName );
        }
        int M;
        ss >> M;
        ss.ignore();
        for (int i = 0; i < M; i++) {
            string route;
            getline(ss, route);
            addRoute(route);

        }
        auto me = Parser::shared_from_this();
        GlobalRunner::get()->addOmni( me );
    }
    void addRoute(string line)
    {
        vector<string> result;
        stringstream ss(line);
        while( ss.good() ) {
            string substr;
            getline( ss, substr, ' ' );
            result.push_back( substr );
        }
        mapData->routes[result[0]]= result[1];
    }
private:

};

string testCase = "StopArea:ABDU\n\
StopArea:ABLA\n\
3\n\
StopArea:ABDU,\"Abel Durand\",,47.22019661,-1.60337553,,,1,\n\
StopArea:ABLA,\"Avenue Blanche\",,47.22973509,-1.58937990,,,1,\n\
StopArea:ACHA,\"Angle Chaillou\",,47.26979248,-1.57206627,,,1,\n\
2\n\
StopArea:ABDU StopArea:ABLA\n\
StopArea:ABLA StopArea:ACHA";

#define cin ss



int main()
{
    stringstream ss (testCase);
    Main* m= new Main(ss);
    Main::get(m);

    m->run();
    std::cerr<<"Starting"<<std::endl;
    GlobalRunner::get()->update();
    std::cerr<<"Done"<<std::endl;
}



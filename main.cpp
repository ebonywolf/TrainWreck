//#include <bits/stdc++.h>

#include <any>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Apollo/Coord.h"
#include "Apollo2/Process.h"
#include "Apollo2/SpecialEntities.h"
#include "out.h"

using namespace std;
using namespace pg;
/*
sf::Color defaultColor = sf::Color(255,0,0,255);
Coord defaultSize = Coord(5,5);

int MAX_Y=500;
Renderer renderer = Renderer({500,MAX_Y});;

vector<Spriteptr> sprites;

struct Square: public sf::RectangleShape {
    Square(Coord size, Coord position, sf::Color color)
    {
        this->setPosition( position.x, position.y );
        this->setFillColor( sf::Color( color ) );
        this->setSize( { size.x, size.y } );
    }
};
Spriteptr createSquare(Coord position, Coord size)
{
    position.y = MAX_Y - position.y;
    Spriteptr sprite = Spriteptr(new Square( size, position,defaultColor));
    sprites.push_back(sprite);
    return sprite;
}

void drawCurve(Axles axles, double maxPrecision=100)
{
    for (int i=0; i<axles.size() -1; i++) {
        Coord current = axles[i];
        Coord next = axles[i+1];
        double xvar = (next.y - current.x)/maxPrecision;
        for (double precision=0; precision < maxPrecision; precision++) {
            double x_position =  current.x + xvar*precision ;
            Coord currentPosition = {x_position, 10};
            renderer.add(createSquare(currentPosition,defaultSize));

            for (int j=0; j<axles.size(); j++) {
                if(i==j)
                    continue;
                Coord j_axle = axles[j];

                double size = axles.size();
                //       double totalDistance= current.distance(x_position);
                //totalDistance+= j_axle
            }
        }
        // Coord position = {current.x}
    }
}

*/

inline std::string className(const std::string& prettyFunction)
{
    size_t colons = prettyFunction.find("::");
    if (colons == std::string::npos)
        return "::";
    size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
    size_t end = colons - begin;

    return prettyFunction.substr(begin,end);
}

#define __CLASS_NAME__ className(__PRETTY_FUNCTION__)


struct Ack: public GenericData<Ack>{
};

struct BoxInfo: public GenericData<BoxInfo>{
    pg::Coord pos,dim;
    friend ostream& operator<<(ostream& os, const BoxInfo& b){
        os<<b.pos;
        return os;
    }

};

class BoxDrawer: public GenericEntity<BoxDrawer>{
public:
    BoxDrawer():GenericEntity(__CLASS_NAME__,drawBox){
    }

    static Ack drawBox(BoxInfo box){
        std::cerr<<"Got box:"<<box<<std::endl;
    }
};



int main(int argc,char** argv)
{
    try{
        Entityptr context = ContextCreator::createFromJson("test.json");
        BoxInfo box;
        Entityptr alce= Entityptr(new Entity());
        alce->addOmni("BoxCreator",context);
        std::cerr<<"Handlers:"<<context->_eurus.size()<<std::endl;
        for(auto& it:context->_eurus)
        {
            std::cerr<<"Pair for:"<<it.first<<std::endl;
        }
        alce->send<Ack>(box);
        context->run();

    }catch(runtime_error& e){
        std::cout<<e.what()<<std::endl;
    }
   // Ack ack =context->send<Ack>(box);
    return 0;
}

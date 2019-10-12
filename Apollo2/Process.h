
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <fstream>
#include <future>

#include <json/json.h>
#include "Data.h"
namespace pg
{

template<typename T>
struct enable_shared_from_this_virtual;

class enable_shared_from_this_virtual_base : public std::enable_shared_from_this<enable_shared_from_this_virtual_base>
{
    typedef std::enable_shared_from_this<enable_shared_from_this_virtual_base> base_type;
    template<typename T>
    friend struct enable_shared_from_this_virtual;

    std::shared_ptr<enable_shared_from_this_virtual_base> shared_from_this()
    {
        return base_type::shared_from_this();
    }
    std::shared_ptr<enable_shared_from_this_virtual_base const> shared_from_this() const
    {
        return base_type::shared_from_this();
    }
};

template<typename T>
struct enable_shared_from_this_virtual: virtual enable_shared_from_this_virtual_base {
    typedef enable_shared_from_this_virtual_base base_type;

public:
    std::shared_ptr<T> shared_from_this()
    {
        std::shared_ptr<T> result(base_type::shared_from_this(), static_cast<T*>(this));
        return result;
    }

    std::shared_ptr<T const> shared_from_this() const
    {
        std::shared_ptr<T const> result(base_type::shared_from_this(), static_cast<T const*>(this));
        return result;
    }
};
/////////////////////////////



struct Process : DataPair {//Defines function
    Process( )
    {

    }
    operator DataPair() const
    {
        return *this;

    }

};
using Processptr = std::shared_ptr<Process>;

template<class INPUT, class OUTPUT>
struct GenericProcess: Process {

    GenericProcess(std::function<OUTPUT(INPUT)> func):func(func)
    {
    }

private:

    std::function<OUTPUT(INPUT)> func;

};


}


namespace std
{
template <>
struct hash<pg::Process> {
    std::size_t operator()(const pg::Process& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;
        return hash<string>()(k.getHashKey());

    }
};
}

namespace pg
{


class Entity;
using Entityptr = std::shared_ptr<Entity>;



class ProcessList :public std::unordered_set<Processptr>
{
public:
    ProcessList()
    {
    }


private:



};


struct EntityMap : std::unordered_map<DataPair,Entityptr> {
    using  std::unordered_map<DataPair,Entityptr>::unordered_map;

    Entityptr operator()(const Datatype& from,const Datatype& to) const
    {
        DataPair par (from,to);
        return at(par);
    }


};



struct Entity : public enable_shared_from_this_virtual<Entity> {//Defines object that runs many functions and has many dataTypes
    using ContextMap = std::unordered_map<std::string,Entityptr>;

    ContextMap _subContext;
    Entityptr _context;
    EntityMap methods;
    ProcessList processes;

    Entity() {
        _context = getGlobal();
    };

    template <class ...T>
    Entity(T...t)
    {
        addProcess(t...);
    };

    template <class INPUT, class OUTPUT>
    void addProcess( OUTPUT(func)(INPUT) )
    {
        Processptr p = Processptr (new GenericProcess<INPUT,OUTPUT>(func));
        processes.insert(p);
    }


    template <class INPUT, class OUTPUT,class ...T>
    void addProcess(OUTPUT(func)(INPUT), T ...t)
    {
        addProcess(func);
        addProcess(t...);
    }

    ProcessList getProcess()
    {
        return processes;
    }
    bool hasMethod(Datatype from, Datatype to) const
    {
        DataPair par (from,to);
        return methods.count(par);

    }

    void addContext(const Entityptr obj)
    {
        for(auto& proc: obj->getProcess()) {
            auto from  = proc->getFrom();
            auto to =  proc->getTo();

            if( hasMethod(from,to) ) {
                methods(from,to) = obj;
                //Todo switch this map with something else
            } else {
                throw std::runtime_error("Context with more than handler");

            }
        }
    }

    Data send(const Data& sentData,const Datatype receiveType )const
    {
        auto toType = sentData.getType();
        if(!hasMethod(toType,receiveType)) {
            throw std::runtime_error("Context does not have method");
        }
    }

    template<class T>
    Data send(const Data& sentData )const
    {
        T received;
        const auto fromType = received.getType();
        return send(sentData, fromType);
    }

//   virtual std::string getName() = 0;



    Entityptr getSubContext(std::string name) const {
        if(_subContext.count(name)){
            return _subContext.at(name);
        }else{
            throw std::runtime_error(std::string("entity not present in context: ")+name);
        }
    }
    Entityptr getContext(std::string name) const {
        return _context->getSubContext(name);
    }

   static Entityptr getGlobal(){
        static Entityptr global;
        if(!global){
            global = Entityptr(new Entity(PlaceHolder()));
            global->_context = global;//shared ptr pointing to itself,
        }

        return global;

    }

protected:
    struct PlaceHolder{
    };
    Entity(PlaceHolder){//Especial constructor

    }
};


#define TO_STR(X) #X
template <class T>
class GenericEntity: public Entity{
    std::string getName() const{
        return TO_STR(T);
    }
protected:
    static Entityptr _instance;
    GenericEntity(PlaceHolder){
        auto global = getGlobal();
        global.addEntity
    }

} ;

template <class T>
Entityptr GenericEntity<T>::_instance = std::make_shared<GenericEntity<T>>(PlaceHolder());


struct ContextCreator {

    static Entityptr addfromJson(Json::Value val){
        Entityptr novo = Entityptr();
        std::cerr<<"Getting from:"<<val.toStyledString()<<std::endl;
        for(auto name :val.getMemberNames())
        {
            Json::Value ent_val =val[name];
            if( ent_val.isObject() ){
                 auto obj = addfromJson(ent_val);
                 auto objectContext = Entity::getGlobal()->getContext(name);
                 objectContext->addContext(obj );
                 novo->addContext(objectContext);

            }else{
               // throw "foo";
            }

        }
        return novo;
    }

   static Entityptr createFromJson(std::string file)
    {
        std::ifstream in(file);
        if(!in.is_open()) {
            throw "unable to open file";
        }
        Json::Value val;
        in>>val;
        return addfromJson(val);
    }

};

/*
template <class MY_TYPE>
struct Common: public enable_shared_from_this_virtual<MY_TYPE>{

    template<class T>
    static std::shared_ptr<MY_TYPE> getRoot(T t)
    {
        return std::static_pointer_cast<MY_TYPE>(t);
    }

   // template<class T>
    std::shared_ptr<MY_TYPE> getRoot(){
    //    return std::static_pointer_cast<MY_TYPE>(t);
        return  std::static_pointer_cast<MY_TYPE>( this->shared_from_this() );
             //   enable_shared_from_this_virtual<MY_TYPE>::shared_from_this();
    }
};


class Attributes{
public:
    using Keyset = std::unordered_set<std::string>;
    Attributes(){//TODO delete default cons
    };
    Attributes(Keyset keys):_keys(keys){}

protected:
    std::unordered_map<std::string, std::any> _values;
    Keyset _keys;
};


template <class MY_TYPE>
class Object: public Common<MY_TYPE>{
public:
    Object(){}
    template<class... T>
    Object(T&&... t ){
    }
private:
    template<class F, class... T>
    void __init(F&& f, T&&... t){
        std::string key = typeof(f);

        _attributes._keys.insert( typeof(f));
    }

    const Attributes _attributes;
};



template <class MY_TYPE>
struct Singleton: public Object<MY_TYPE>
{
    static std::shared_ptr<MY_TYPE> get( MY_TYPE* val=0  ){

        static std::shared_ptr<MY_TYPE> me;
        if(val){
            me.reset(val);
        }
        if(!me){
            me=std::make_shared<MY_TYPE>();
        }
        return me;
    }

protected:
    Singleton(){}

};

template <class MY_TYPE>
struct MultiInstance : public Object<MY_TYPE> {
     static std::shared_ptr<MY_TYPE> get(){
        //TODO guarantee cleanup
        return std::make_shared<MY_TYPE>();
    }
};


*/

}


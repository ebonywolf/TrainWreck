
#pragma once

namespace pg
{

using DataId = int;
struct Datatype { //defines dataTypes, not data itself

    static const Datatype NULL_DATA;

    Datatype():name("null")
    {
    }
    Datatype(std::string name):name(name)
    {
    }
    bool operator==(const Datatype& other)const
    {
        return name == other.name;
    }
    bool operator<(const Datatype& other)const
    {
        return name < other.name;
    }
    std::string getName()const
    {
        return name;
    };
    friend std::ostream& operator<<(std::ostream& os, const Datatype& data )
    {
        os<<data.name;
        return os;
    }
private:
    std::string name;
};
const Datatype Datatype::NULL_DATA = Datatype();

struct DataPair {
    DataPair() {}
    DataPair(Datatype from, Datatype to):from(from),to(to)
    {
    }


    Datatype getFrom()const
    {
        return from;
    }
    Datatype getTo()const
    {
        return to;
    }
    std::string getHashKey()const
    {
        if(to < from) {
            return to.getName() + from.getName() ;
        } else {
            return from.getName() + to.getName() ;
        }
    }
    bool operator==(const DataPair& other)const
    {
        return getHashKey() == other.getHashKey();
    }
private:
    Datatype from,to;

};

struct Base_Data {

};

struct Data : protected std::unique_ptr<Base_Data> {

    Data()
    {
    }

    virtual Datatype getType() const
    {
        return Datatype::NULL_DATA;
    };
    friend std::ostream& operator<<(std::ostream& os, const Data& data )
    {
        os<<"Data:"<<data.getType();
        return os;
    }

};


template <class T>
struct GenericData: public Data {
    virtual Datatype getType() const
    {
        return Datatype(typeid(T).name());
    }


    friend std::ostream& operator<<(std::ostream& os, const GenericData<T>& data )
    {
        os<<"Data:"<<data.getType()<<":"<<data.get();

        return os;
    }


};
;

}

namespace std
{
template <>
struct hash<pg::Datatype> {
    std::size_t operator()(const pg::Datatype& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        return hash<string>()(k.getName());

    }
};
template <>
struct hash<pg::DataPair> {
    std::size_t operator()(const pg::DataPair& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;
        return hash<string>()(k.getHashKey());

    }
};


}

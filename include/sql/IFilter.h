#ifndef IFilter_h
#define IFilter_h

#include <string>
#include <vector>
#include <iterator>
#include <io/Exceptions/NotImplemented.h>

namespace Filters{
    enum SqlComparator{
        Equals,
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual
    };

    enum SqlDataType{
        INTEGER,
        REAL,
        TEXT,
        BLOB
    };

    enum WhereUUID{
        Id,
        InputId,
        ParticleId,
        ScaleFactorId
    };

    class IFilter
    {
    private:
        std::string sqlComparatorText(SqlComparator sql);
    protected:
        std::vector<std::string> filters_;
        void AddFilterStatement( std::string column, std::string value, SqlDataType valueType, SqlComparator comparator );
    public:
        std::string Filter();
        IFilter(/* args */);
        ~IFilter();
    };    
}

#endif

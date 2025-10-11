#include <sql/IFilter.h>

using namespace std;
using namespace Filters;

IFilter::IFilter(/* args */)
{
}
    
IFilter::~IFilter()
{
}

string IFilter::sqlComparatorText(SqlComparator sql){
    switch (sql)
    {
    case SqlComparator::Equals:
        return "=";
    case SqlComparator::LessThan:
        return "<";
    case SqlComparator::GreaterThan:
        return ">";
    case SqlComparator::LessThanOrEqual:
        return "<=";
    case SqlComparator::GreaterThanOrEqual:
        return ">=";
    case SqlComparator::Like:
        return " LIKE ";
    default:
        throw NotImplementedException();
    }
}

void IFilter::AddFilterStatement(string column, string value, SqlDataType valueType, SqlComparator comparator){
    string filterStr;
    filterStr.append(column);
    filterStr.append( sqlComparatorText(comparator) );
    if (valueType == SqlDataType::TEXT || valueType == SqlDataType::BLOB){
        filterStr.append("'");
        filterStr.append(value);
        filterStr.append("'");
    } else{
        filterStr.append(value);
    }
    filters_.push_back(filterStr);
}

string IFilter::Filter(){
    string fullFilter;
    for( auto f = filters_.begin(); f != filters_.end(); ++f ){
        fullFilter.append( *f );
        if (std::next(f) != filters_.end()){
            fullFilter.append(" AND ");
        }
    }
    return fullFilter;
}

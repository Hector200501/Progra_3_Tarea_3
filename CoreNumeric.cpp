#include <iostream>
#include <concepts>
#include <iterator>
#include <vector>
#include <string>
using namespace std;

//Iterable
template < typename C >
concept Iterable = requires ( C c ) {
    std :: begin ( c ) ;
    std :: end ( c ) ;
};

//Addable
template < typename T >
concept Addable = requires ( T a , T b ) {
    { a + b } -> std :: same_as <T >;
};


//Divisible
template < typename T >
concept Divisible = requires ( T a , std :: size_t n ) {
    { a / n } -> std :: same_as <T >;
};

//Multipliable - Concept propio
template < typename T >
concept Multipliable = requires ( T a , T b ){
    { a * b } -> std :: same_as <T >; //Se aplica para el transform_reduce
};

//Comparable - Concept propio
template < typename T >
concept Comparable = requires ( T a , T b ){
    { a > b } -> std :: same_as <bool>;
};


namespace core_numeric {
    //Funcion sum
    template < Iterable C >
    requires Addable < typename C :: value_type >
    auto sum ( const C & container ){
        using T = typename C :: value_type ;
        T result {}; // Para tipos numericos (int , double , float )
        // se inicializa en 0.
        // Para clases se llama al constructor por defecto
        for ( const auto & value : container )
            result = result + value ;
        return result ;
    }

    //Funcion mean
    template < Iterable C >
    requires Addable < typename C :: value_type > && Divisible < typename C :: value_type >
    auto mean ( const C & container ){
        auto total = sum ( container ) ;
        return total / container.size() ;
    }

    //Funcion variance
    template < Iterable C >
    requires Addable < typename C :: value_type > && Divisible < typename C :: value_type >
    auto variance ( const C & container ){
        using T = typename C::value_type;
        auto prom= mean ( container ) ;
        T sumatoria{};
        for ( const auto & value : container ) {
            auto desviacion= value - prom ;
            sumatoria += (desviacion * desviacion);
        }
        return sumatoria/container.size();
    }

    //Funcion max
    template < Iterable C >
    requires Comparable < typename C :: value_type >
    auto max ( const C & container ) {
        using T = typename C::value_type;
        T valormax=*begin(container);
        for ( const auto & value : container ) {
            if ( value > valormax ) {
                valormax = value;
            }
        }
        return valormax;
    }

    //Funcion transform_reduce
    template < Iterable C, typename F >
    requires Addable < typename C :: value_type >
    auto transform_reduce( const C & container, F f ) {
        using T = typename C :: value_type ;
        T result {};
        for ( const auto & value : container )
            result += f(value) ;
        return result ;
    }
}

int main() {
    std::vector<double> v {2.0, 3.0, 4.0};
    auto r = core_numeric::transform_reduce(v, [](double x) {
        return x * x;
    });
    cout << r << endl;
}




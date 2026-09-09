#include <iostream>
#include <concepts>
#include <iterator>
#include <vector>
#include <string>
#include <type_traits>
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
    template < typename... Args >
    requires (Addable<Args> && ...)
    auto sum_variadic ( Args... args ) {
        return ( args + ... ); // fold expression
    }

    // mean_variadic: reutiliza sum_variadic (igual que mean reutiliza sum).
    // Requerimiento 7: usa if constexpr para diferenciar el comportamiento
    // entre tipos enteros y tipos flotantes en tiempo de compilacion.
    template < typename... Args >
    requires (Addable<Args> && ...)
    auto mean_variadic ( Args... args ) {
        using T = std::common_type_t<Args...>;
        constexpr std::size_t n = sizeof...(args);
        auto total = sum_variadic ( args... );

        if constexpr ( std::is_integral_v<T> ) {
            // Caso enteros: division entera, se trunca el resultado.
            return static_cast<T>(total) / static_cast<T>(n);
        } else {
            // Caso flotantes: division exacta.
            return static_cast<T>(total) / static_cast<T>(n);
        }
    }

    // variance_variadic: reutiliza mean_variadic (igual que variance reutiliza mean).
    // Ademas de Addable, exige el concept propio Multipliable sobre el tipo
    // comun de los argumentos (se usa para elevar al cuadrado cada desviacion).
    template < typename... Args >
    requires (Addable<Args> && ...) && Multipliable<std::common_type_t<Args...>>
    auto variance_variadic ( Args... args ) {
        using T = std::common_type_t<Args...>;
        T prom = static_cast<T>( mean_variadic ( args... ) );
        // Fold expression: suma de (xi - prom)^2 para cada argumento
        auto sumatoria = ( ( ( static_cast<T>(args) - prom ) * ( static_cast<T>(args) - prom ) ) + ... );
        return sumatoria / static_cast<T>( sizeof...(args) );
    }

    // max_variadic: usa el concept Comparable y una fold expression (comma-fold)
    // que va comparando y actualizando el maximo con cada argumento.
    template < typename First, typename... Rest >
    requires Comparable<std::common_type_t<First, Rest...>>
    auto max_variadic ( First first, Rest... rest ) {
        using T = std::common_type_t<First, Rest...>;
        T resultado = static_cast<T>(first);
        ( ( resultado = ( static_cast<T>(rest) > resultado ? static_cast<T>(rest) : resultado ) ), ... );
        return resultado;
    }
}
int main() {
    std::vector<double> v {2.0, 3.0, 4.0};
    auto r = core_numeric::transform_reduce(v, [](double x) {
        return x * x;
    });
    cout << r << endl;
}




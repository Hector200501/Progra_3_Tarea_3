// test.cpp - Requerimiento 8:
// Este archivo es autocontenido (incluye una copia de los concepts y de las funciones de core_numeric) para poder compilarse de forma independiente:
// Cada bloque muestra:
//   1) Un caso que SI compila.
//   2) Un caso que NO compila (comentado), con una explicacion de que concept falla y por que.

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
    { a * b } -> std :: same_as <T >;
};

//Comparable - Concept propio
template < typename T >
concept Comparable = requires ( T a , T b ){
    { a > b } -> std :: same_as <bool>;
};

namespace core_numeric {

    template < Iterable C >
    requires Addable < typename C :: value_type >
    auto sum ( const C & container ){
        using T = typename C :: value_type ;
        T result {};
        for ( const auto & value : container )
            result = result + value ;
        return result ;
    }

    template < Iterable C >
    requires Addable < typename C :: value_type > && Divisible < typename C :: value_type >
    auto mean ( const C & container ){
        auto total = sum ( container ) ;
        return total / container.size() ;
    }

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
        return ( args + ... );
    }

    template < typename... Args >
    requires (Addable<Args> && ...)
    auto mean_variadic ( Args... args ) {
        using T = std::common_type_t<Args...>;
        constexpr std::size_t n = sizeof...(args);
        auto total = sum_variadic ( args... );
        if constexpr ( std::is_integral_v<T> ) {
            return static_cast<T>(total) / static_cast<T>(n);
        } else {
            return static_cast<T>(total) / static_cast<T>(n);
        }
    }

    template < typename... Args >
    requires (Addable<Args> && ...) && Multipliable<std::common_type_t<Args...>>
    auto variance_variadic ( Args... args ) {
        using T = std::common_type_t<Args...>;
        T prom = static_cast<T>( mean_variadic ( args... ) );
        auto sumatoria = ( ( ( static_cast<T>(args) - prom ) * ( static_cast<T>(args) - prom ) ) + ... );
        return sumatoria / static_cast<T>( sizeof...(args) );
    }

    template < typename First, typename... Rest >
    requires Comparable<std::common_type_t<First, Rest...>>
    auto max_variadic ( First first, Rest... rest ) {
        using T = std::common_type_t<First, Rest...>;
        T resultado = static_cast<T>(first);
        ( ( resultado = ( static_cast<T>(rest) > resultado ? static_cast<T>(rest) : resultado ) ), ... );
        return resultado;
    }
}

// ============================================================
// Clases de prueba (Requerimiento 8)
// ============================================================

// Coordenadas: cumple Addable, Divisible, Multipliable y Comparable.
// Por lo tanto puede usarse en TODOS los algoritmos de core_numeric.
struct Coordenadas {
    double x{};
    double y{};

    Coordenadas operator+(const Coordenadas& o) const {
        return { x + o.x, y + o.y };
    }
    Coordenadas& operator+=(const Coordenadas& o) {
        x += o.x; y += o.y;
        return *this;
    }
    Coordenadas operator/(std::size_t n) const {
        return { x / static_cast<double>(n), y / static_cast<double>(n) };
    }
    Coordenadas operator*(const Coordenadas& o) const {
        return { x * o.x, y * o.y }; // multiplicacion componente a componente
    }
    Coordenadas operator-(const Coordenadas& o) const {
        return { x - o.x, y - o.y };
    }
    // Se compara por magnitud (distancia al origen)
    bool operator>(const Coordenadas& o) const {
        return (x * x + y * y) > (o.x * o.x + o.y * o.y);
    }
    bool operator==(const Coordenadas& o) const {
        return x == o.x && y == o.y;
    }
    void print() const { cout << "(" << x << ", " << y << ")"; }
};

// Vector3D: solo define operator+. Cumple Addable, pero NO Divisible
// (no hay operator/) ni Comparable (no hay operator>).
struct Vector3D {
    double x{}, y{}, z{};

    Vector3D operator+(const Vector3D& o) const {
        return { x + o.x, y + o.y, z + o.z };
    }
    // Metodo para comparar objetos (igualdad), pedido por el enunciado.
    bool equals(const Vector3D& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
    void print() const { cout << "(" << x << ", " << y << ", " << z << ")"; }
};

// Esfera: NO define operator+ (no es Addable). Si define operator> segun
// su volumen, por lo que SI cumple Comparable.
struct Esfera {
    double radio{};

    bool operator>(const Esfera& o) const { return radio > o.radio; }
    bool equals(const Esfera& o) const { return radio == o.radio; }
    void print() const { cout << "Esfera(r=" << radio << ")"; }
};

int main() {

    // ============================================================
    // 1) Tipos fundamentales: int, double
    // ============================================================
    std::vector<int> enteros{ 1, 2, 3, 4 };
    std::vector<double> flotantes{ 1.0, 2.7, 0.3, 5.5 };

    auto suma_int = core_numeric::sum(enteros);              // OK: int es Addable
    auto media_double = core_numeric::mean(flotantes);       // OK: double es Addable y Divisible
    auto var_double = core_numeric::variance(flotantes);     // OK: double es Addable y Divisible
    auto max_double = core_numeric::max(flotantes);          // OK: double es Comparable
    auto tr = core_numeric::transform_reduce(flotantes, [](double x) { return x * x; }); // OK

    cout << "sum(enteros) = " << suma_int << endl;
    cout << "mean(flotantes) = " << media_double << endl;
    cout << "variance(flotantes) = " << var_double << endl;
    cout << "max(flotantes) = " << max_double << endl;
    cout << "transform_reduce(flotantes, x*x) = " << tr << endl;

    // ============================================================
    // 2) Tipo fundamental: std::string
    // ============================================================
    std::vector<std::string> palabras{ "Hola", "Mundo", "Cpp" };

    // sum(palabras) SI compila: std::string tiene operator+ que devuelve
    // std::string (same_as<T> se cumple), asi que Addable<std::string> es true.
    auto concatenado = core_numeric::sum(palabras);
    cout << "sum(palabras) = " << concatenado << endl;

    // max(palabras) SI compila: std::string tiene operator> (orden lexicografico),
    // por lo tanto Comparable<std::string> es true.
    auto max_palabra = core_numeric::max(palabras);
    cout << "max(palabras) = " << max_palabra << endl;

    // mean(palabras) y variance(palabras) NO COMPILAN:
    // std::string no tiene operator/(std::string, size_t), por lo tanto
    // NO cumple el concept Divisible. requires Addable<...> && Divisible<...>
    // falla porque Divisible<std::string> es false.
    //
    // auto media_palabras = core_numeric::mean(palabras);       // ERROR: Divisible<string> no se cumple
    // auto var_palabras   = core_numeric::variance(palabras);   // ERROR: Divisible<string> no se cumple

    // ============================================================
    // 3) Objetos de clase: Coordenadas (cumple los 4 concepts)
    // ============================================================
    std::vector<Coordenadas> puntos{ {1.0, 2.0}, {3.0, 4.0}, {0.0, 1.0} };

    auto suma_coord = core_numeric::sum(puntos);      // OK: Addable<Coordenadas>
    auto media_coord = core_numeric::mean(puntos);    // OK: Addable + Divisible
    auto var_coord = core_numeric::variance(puntos);  // OK: Addable + Divisible
    auto max_coord = core_numeric::max(puntos);       // OK: Comparable (compara por magnitud)

    cout << "sum(puntos) = ";      suma_coord.print();  cout << endl;
    cout << "mean(puntos) = ";     media_coord.print(); cout << endl;
    cout << "variance(puntos) = "; var_coord.print();   cout << endl;
    cout << "max(puntos) = ";      max_coord.print();   cout << endl;
    cout << "mean(puntos) == mean(puntos) ? " << std::boolalpha
         << (media_coord == media_coord) << endl; // uso del metodo de comparacion

    // sum_variadic tambien funciona con Coordenadas gracias a Addable<Coordenadas>
    auto suma_var_coord = core_numeric::sum_variadic(Coordenadas{1,1}, Coordenadas{2,2}, Coordenadas{3,3});
    cout << "sum_variadic(Coordenadas...) = "; suma_var_coord.print(); cout << endl;

    // ============================================================
    // 4) Objetos de clase: Vector3D (solo cumple Addable)
    // ============================================================
    std::vector<Vector3D> vectores{ {1,0,0}, {0,1,0}, {0,0,1} };

    auto suma_vec = core_numeric::sum(vectores); // OK: Vector3D tiene operator+ (Addable)
    cout << "sum(vectores) = "; suma_vec.print(); cout << endl;
    cout << "sum(vectores) == sum(vectores)? " << std::boolalpha
         << suma_vec.equals(suma_vec) << endl; // uso del metodo equals() para comparar

    // mean(vectores) y variance(vectores) NO COMPILAN:
    // Vector3D no define operator/(Vector3D, size_t), por lo tanto NO cumple
    // Divisible<Vector3D>. La clausula requires Addable<...> && Divisible<...> falla.
    //
    // auto media_vec = core_numeric::mean(vectores);     // ERROR: Divisible<Vector3D> no se cumple
    // auto var_vec   = core_numeric::variance(vectores);  // ERROR: Divisible<Vector3D> no se cumple

    // max(vectores) NO COMPILA:
    // Vector3D no define operator>, por lo tanto NO cumple Comparable<Vector3D>.
    //
    // auto max_vec = core_numeric::max(vectores); // ERROR: Comparable<Vector3D> no se cumple

    // ============================================================
    // 5) Objetos de clase: Esfera (NO cumple Addable, SI cumple Comparable)
    // ============================================================
    std::vector<Esfera> esferas{ {1.0}, {2.5}, {0.7} };

    auto max_esfera = core_numeric::max(esferas); // OK: Esfera define operator> (Comparable)
    cout << "max(esferas) = "; max_esfera.print(); cout << endl;
    cout << "max(esferas) == max(esferas)? " << std::boolalpha
         << max_esfera.equals(max_esfera) << endl;

    // sum(esferas), mean(esferas), variance(esferas) y
    // transform_reduce(esferas, f) NO COMPILAN:
    // Esfera no define operator+, por lo tanto NO cumple Addable<Esfera>.
    // Todas estas funciones requieren Addable<typename C::value_type>.
    //
    // auto suma_esferas = core_numeric::sum(esferas);        // ERROR: Addable<Esfera> no se cumple
    // auto media_esferas = core_numeric::mean(esferas);      // ERROR: Addable<Esfera> no se cumple
    // auto var_esferas = core_numeric::variance(esferas);    // ERROR: Addable<Esfera> no se cumple

    // ============================================================
    // 6) Requerimiento 7 (if constexpr): mismo algoritmo, distinto
    //    comportamiento en tiempo de compilacion segun el tipo.
    // ============================================================
    auto media_entera   = core_numeric::mean_variadic(1, 2, 3, 4);          // T=int    -> division entera
    auto media_flotante = core_numeric::mean_variadic(1.0, 2.0, 3.0, 5.0);  // T=double -> division exacta
    cout << "mean_variadic(1,2,3,4) [int]        = " << media_entera   << endl;
    cout << "mean_variadic(1.0,2.0,3.0,5.0) [double] = " << media_flotante << endl;

    return 0;
}
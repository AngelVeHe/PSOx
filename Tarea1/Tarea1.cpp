#include "PSOx.hpp"

using namespace std;

int main(void)
{

    uint64_t dim = 2;
    uint32_t n_particles = 30;

    vector<double> X_min(dim, -1.0);
    vector<double> X_max(dim, 1.0);

    PSOx optimizer(n_particles, dim, "LDIW", X_max, X_min, 2, 2);

    auto sphere_function = [](const vector<double>& x) -> double {
        double sum = 0.0;
        for (double val : x) {
            sum += (val * val);
        }
        return sum;
    };

    auto himmelblau_function = [](const vector<double>& pos) -> double {
    double x = pos[0];
    double y = pos[1];
    
    double term1 = (x * x) + y - 11.0;
    double term2 = x + (y * y) - 7.0;
    
    return (term1 * term1) + (term2 * term2);
    };

    try {
        optimizer.solve(sphere_function, 1000);

        auto solucion = optimizer.get_results();
        
        cout << "=== Resultados de Optimizacion ===" << endl;
        cout << "Mejor Fitness : " << solucion.best_fitness << endl;
        cout << "Coordenada X  : " << solucion.best_position[0] << endl;
        cout << "Coordenada Y  : " << solucion.best_position[1] << endl;
    } catch (const exception& e) {
        cerr << "Error en ejecución" << e.what() << endl;
    }


    vector<double> X_min_him(dim, -6.0);
    vector<double> X_max_him(dim, 6.0);
    PSOx optimizer_him(n_particles, dim, "LDIW", X_max_him, X_min_him, 2, 2);
    try {
        optimizer_him.solve(himmelblau_function, 1000);
        auto solucion_him = optimizer_him.get_results();

        cout << "=== Resultados de Optimizacion ===" << endl;
        cout << "Mejor Fitness : " << solucion_him.best_fitness << endl;
        cout << "Coordenada X  : " << solucion_him.best_position[0] << endl;
        cout << "Coordenada Y  : " << solucion_him.best_position[1] << endl;
    } catch (const exception& e) {
        cerr << "Error en ejecución" << e.what() << endl;
    }

    return 0;

}



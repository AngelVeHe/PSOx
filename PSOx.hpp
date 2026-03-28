#ifndef PSOX_HPP
#define PSOX_HPP

#include <cinttypes>
#include <vector>
#include <string>
#include <random>
#include <stdexcept>
#include <limits> 
#include <iostream>

using namespace std;


class PSOx
{
    private:
        
        struct Particle{
            vector<double> x;
            vector<double> best_x;
            vector<double> v;
            double personal_best;

            Particle(uint64_t dim) : x(dim), v(dim), best_x(dim), personal_best(numeric_limits<double>::max()) {}
        };

        struct Swarm{
            vector<Particle> particles;
            vector<double> global_best_x;
            double global_best; 
            uint32_t n_particles;
            
            Swarm() : global_best(numeric_limits<double>::max()) {}

        };

        struct Result {
            double best_fitness;
            vector<double> best_position;
        };

        
        Swarm swarm_data;               //Vector de particulas
        uint64_t max_iter = 500;        //Maximo de iteraciones
        uint64_t n;                     //Dimension del problema 
        string inertia_type;            //Tipo de inercia
        double inertia;
        double max_inertia = 0.9;
        double min_inertia = 0.4;
        double c1, c2;                  //Constantes de aceleración           
        vector<double> X_max_limits;    //Limites del espacio de busqueda
        vector<double> X_min_limits;
        

    public:
        PSOx();
        PSOx(uint32_t n_particles, uint64_t dim, string inertia_mode, 
            const vector<double>& X_max, const vector<double>& X_min,
            double acc_c1 = 2.1, double acc_c2 = 2.1);
        ~PSOx();
        
        
        template <typename FitnessFunc>
        void solve(FitnessFunc& fitness_evaluation, uint64_t t_max);
        double get_global_best();
        Result get_results() const;

};


inline PSOx::PSOx()
{};

inline PSOx::~PSOx()
{};


inline PSOx::PSOx(uint32_t n_particles, uint64_t dim, string inertia_mode,
                const vector<double>& X_max, const vector<double>& X_min,
                double acc_c1, double acc_c2):
                n(dim), 
                X_max_limits(X_max),
                X_min_limits(X_min)
{

    //Limites de las constantes de aceleracion
    if (acc_c1 + acc_c2 > 4.1 || (acc_c1 < 0.0 || acc_c2 < 0)) {
        throw invalid_argument("Invalid acceleration constants. 0 <= (acc_c1 + acc_2) <= 4.1");
    }
     
    c1 = acc_c1;
    c2 = acc_c2;
    
    
    //Inicializacion del tipo de inercia
    if (inertia_mode == "No") {
        inertia_type = inertia_mode;
        inertia = 1.0;
    } else if (inertia_mode == "LDIW") {
        inertia_type = inertia_mode;
        inertia = max_inertia;
    } else {
        throw invalid_argument("Invalid Inertia Type. Valid modes: 'No' or 'LWID'");
    };

    //Resize el total de particulas al tamaño de particulas y global_mest a INF
    swarm_data.n_particles = n_particles;
    swarm_data.global_best = numeric_limits<double>::max();
    swarm_data.global_best_x.resize(n);

    //Generador de distribución aleatoria uniforme
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    
    for (uint32_t i = 0; i < n_particles; ++i) {
        Particle p(n);

        //Inicializacion de posicion y velocidad
        for (uint64_t j = 0; j < n; ++j) {
            double range = X_max[j] - X_min[j];
            p.x[j] = X_min[j] + dis(gen) * range;
            p.v[j] = dis(gen) / 2 * (range);
        }
        p.best_x = p.x;
        swarm_data.particles.push_back(p);
    }
};



template <typename FitnessFunc>
void PSOx::solve(FitnessFunc& fitness_evaluation, uint64_t t_max)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    for (uint64_t t = 0; t < max_iter; ++t) {

        if (inertia_type == "LDIW") {
            inertia = max_inertia -  ((max_inertia - min_inertia) / max_iter) * t;
        }

        
        for (auto& p : swarm_data.particles) {
            double current_fitness = fitness_evaluation(p.x);


            //Evaluacion del punto x
            if (current_fitness < p.personal_best) {
                p.personal_best = current_fitness;
                p.best_x = p.x;
                
                //Actualizacion del global
                if (current_fitness < swarm_data.global_best) {
                    swarm_data.global_best = current_fitness;
                    swarm_data.global_best_x = p.x;
                }
            }

            //Calculo velocidad
            for (uint64_t i = 0; i < n; ++i) {
                double r1 = dis(gen);
                double r2 = dis(gen);
                double px = p.x[i];
                double pv = p.v[i];
                double pbx = p.best_x[i];
                double gbx = swarm_data.global_best_x[i];
                    

                p.v[i] = inertia * pv + (c1 * r1 * (pbx - px)) + (c2 * r2 * (gbx - px));
                p.x[i] = px + p.v[i];

                if (p.x[i] < X_min_limits[i]) {
                    p.x[i] = X_min_limits[i];
                    p.v[i] *= -0.5; // Rebote y perdida de energia
                } else if (p.x[i] > X_max_limits[i]) {
                    p.x[i] = X_max_limits[i];
                    p.v[i] *= -0.5;
                }
            }       
        }
    }
}



inline double PSOx::get_global_best()
{
    return swarm_data.global_best;
}

inline PSOx::Result PSOx::get_results() const {
    return { swarm_data.global_best, swarm_data.global_best_x };
}

#endif
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

        Swarm swarm_data;
        vector<double> V_maxes;
        vector<double> V_mins;


        //Maximo de iteraciones
        uint64_t max_iter = 500;

        //Dimension del problema
        uint64_t n;
        

        //Tipo de inercia
        string inertia_type;
        double inertia;
        double max_inertia = 0.9;
        double min_inertia = 0.4;

        //Constantes de aceleración
        double c1, c2;
        
        //Global best
        

    public:
        PSOx();
        PSOx::PSOx(u_int32_t n_particles, uint64_t dim, string inertia_mode, double acc_c1 = 2.1, 
                    double acc_c2 = 2.1, const vector<double>& X_max, const vector<double>& X_min);
        ~PSOx();
        


        template <typename FitnessFunc>
        void solve(FitnessFunc& fitness_evaluation) {
            for (uint64_t t = 0; t < max_iter; ++t) {

                if (inertia_type == "LWID") {
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
                            swarm_data.global_best = current_fitness
                            swarm_data.global_best_x = p.x;
                        }
                    }

                    p.v += c1; 

                }
            }
        }

};


inline PSOx::PSOx()
{};

inline PSOx::~PSOx()
{};


inline PSOx::PSOx(u_int32_t n_particles = 50, uint64_t dim, string inertia_mode,
                double acc_c1 = 2.1, double acc_c2 = 2.1, 
                const vector<double>& X_max, const vector<double>& X_min): 
                n(dim)
{

    //Limites de las constantes de aceleracion
    if (acc_c1 + acc_c2 > 4.1 || (acc_c1 < 0.0 || acc_c2 <= 0)) {
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


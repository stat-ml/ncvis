#include <omp.h>
#include <random>
#include <cmath>
#include "../lib/pcg-cpp/include/pcg_random.hpp"
#include "simplexvis.hpp"
#include <chrono>

simplexvis::SimplexVis::SimplexVis(long d, long n_threads, long max_clique_size, 
                                   long random_seed, int n_epochs, int n_init_epochs,
                                   float a, float b, float alpha, float alpha_Q, long* n_noise):
d_(d), M_(M), max_clique_size_(max_clique_size), 
random_seed_(random_seed), n_epochs_(n_epochs), n_init_epochs_(n_init_epochs), 
a_(a), b_(b), alpha_(alpha), alpha_Q_(alpha_Q)
{
    omp_set_num_threads(n_threads);
    n_noise_ = new long[n_epochs];
    long default_noise = 3;
    for (int i=0; i<n_epochs; ++i){
        n_noise_[i] = (n_noise == nullptr)? default_noise:n_noise[i];
    }
}

simplexvis::SimplexVis::~SimplexVis(){
    delete[] n_noise_;
    n_noise_ =  nullptr;
}

void simplexvis::SimplexVis::init_embedding(long N, long M, const long *const E, const float *const W, float* Y){
    // For temporary values
    float* Ys[2];
    Ys[0] = Y;
    Ys[1] = new float[N*d_];
    float* mean = new float[d_];
    float* sigma = new float[d_];

    #pragma omp parallel
    {
    int id = omp_get_thread_num();
    pcg64 pcg(random_seed_ + id);
    std::uniform_real_distribution<float> gen_Y(0, 1);

    #pragma omp for
    for (long i = 0; i < N*d_; ++i){
        Y[i] = gen_Y(pcg);
    }
    // Initialize layout
    for (int init_epoch = 0; init_epoch < n_init_epochs_; ++init_epoch){
        float* Y_old = Ys[init_epoch % 2];
        float* Y_new = Ys[(init_epoch+1) % 2];
        #pragma omp for
        for (long i = 0; i < N*d_; ++i){
            Y_new[i] = 0;
        }
        
        #pragma omp for
        for (long i = 0; i < M; ++i){
            long id = E[2*i];
            long other_id = E[2*i + 1];
            for (long k = 0; k < d_; ++k){
                Y_new[id*d_+k] += alpha * W[i] * Y_old[other_id*d_+k];
            }
        }

        #pragma omp single
        for (long k = 0; k < d_; ++k){
            mean[k] = 0;
            sigma[k] = 0;
        }

        #pragma omp for
        for (long i = 0; i < N; ++i){
            for (long k = 0; k < d_; ++k){
                #pragma omp atomic
                mean[k] += Y_new[i*d_+k];
            }
        }

        #pragma omp single
        for (long k = 0; k < d_; ++k){
            mean[k] /= N;
        }
    
        #pragma omp for
        for (long i = 0; i < N; ++i){
            for (long k = 0; k < d_; ++k){
                float delta2 = Y_new[i*d_+k] - mean[k];
                delta2 *= delta2;
                #pragma omp atomic
                sigma[k] += delta2;
            }
        }

        #pragma omp single
        for (long k = 0; k < d_; ++k){
            sigma[k] /= N;
            sigma[k] = sqrtf(sigma[k]);
        }
        
        #pragma omp for
        for (long i = 0; i < N; ++i){
            for (long k = 0; k < d_; ++k){
                Y_new[i*d_+k] = (Y_new[i*d_+k] - mean[k])/sigma[k];
            }
        }
    }
    if (n_init_epochs_ % 2){
        #pragma omp for
        for (long i = 0; i < N*d_; ++i){
            Ys[0][i] = Ys[1][i];
        }
    }
    }
    
    delete[] Ys[1];
    delete[] mean;
    delete[] sigma;
}

void simplexvis::SimplexVis::optimize(long N, long M, const long *const E, const float *const W, float* Y){
    float Q_cum=0.; 
    #pragma omp parallel
    {
    int id = omp_get_thread_num();
    int n_threads = omp_get_num_threads();
    pcg64 pcg(random_seed_+id);
    // Build layout
    std::uniform_int_distribution<long> gen_ind(0, N-1);

    for (int epoch = 0; epoch < n_epochs_; ++epoch){
        // Hogwild: lock-free parameters reading and writing
        // Smoothly reduce stepsize: step = alpha * (1-t^2), t in [0..1]
        float step = alpha_*(1-(((float)epoch)/n_epochs_)*(((float)epoch)/n_epochs_));
        float Q_copy = Q;
        long cur_noise = n_noise_[epoch];
        #pragma omp for nowait
        for (long i = 0; i < M; ++i){
            // printf("[%d] (%ld, %ld)\n", epoch, E[2*i], E[2*i + 1]);
            long id = E[2*i];
            for (long j = 0; j < cur_noise+1; ++j){
                long other_id;
                // Positive sample
                if (j == 0){
                    other_id = E[2*i + 1];
                // Negative samples
                } else{
                    // Sample id that doesn't coincide with the given one
                    do{
                        other_id = gen_ind(pcg);
                    } while (other_id == id);
                }

                float d2 = d_sqr(Y+id*d_, Y+other_id*d_);
                float Ph = 1/(1+a_*powf(d2, b_));
                // Initialize weight
                float w = W[i];
                if (cur_noise != 0){
                    w *= Ph/(cur_noise*expf(Q_copy));
                    if (j == 0){
                        w = 1/(1+w);    
                    } else {
                        w = -1/(1+1/w);
                    }
                    // Non-blocking write
                    // #pragma omp critical
                    // {
                    // printf("[%d:%d] Ph = %f\n", epoch, omp_get_thread_num(), Ph);
                    // }
                    Q_copy -= w*alpha_Q_;
                    w = 2*w*Ph*a_*b_*powf(d2, b_-1);
                }

                // #pragma omp critical
                // {
                // printf("[%d:%d] Q = %f\n", epoch, omp_get_thread_num(), Q);
                // }
                // Also non-blocking write
                for (long k = 0; k < d_; ++k){
                    float dx_k = Y[other_id*d_+k] - Y[id*d_+k];
                    dx_k = dx_k * w * step;
                    // Clipping to avoid too large stepsize
                    if (dx_k > 4.){
                        dx_k = 4.;
                    } else if (dx_k < -4.){
                        dx_k = -4.;
                    }
                    Y[id*d_+k] += dx_k;
                    Y[other_id*d_+k] -= dx_k;
                }
            }
        }
        #pragma omp atomic
        Q_cum += Q_copy;
        #pragma omp barrier
        #pragma omp single
        {
        Q = Q_cum/n_threads;
        Q_cum = 0;
        }
    }
    }
}

void simplexvis::SimplexVis::fit_transform(long N, long M, const long *const E, const float *const W, float* Y){
    if (N == 0){ 
        throw std::runtime_error("[simplexvis::SimplexVis::fit_transform] There should be at least one node.");
    }
    if (E == nullptr){
        throw std::runtime_error("[simplexvis::SimplexVis::fit_transform] Null pointer provided for edges.");
    }
    if (W == nullptr){
        throw std::runtime_error("[simplexvis::SimplexVis::fit_transform] Null pointer provided for weights.");
    }
    if (Y == nullptr){
        throw std::runtime_error("[simplexvis::SimplexVis::fit_transform] Null pointer provided for output.");
    }

    // printf("==============EDGES============\n");
    // for (long i=0; i<N; ++i){
    //     printf("[");
    //     for (long j=0; j<D; ++j){
    //         printf("%5.2f ", X[j+D*i]);
    //     }
    //     printf("]\n");
    // }
    // printf("===============================\n");

    float Q=0.;
    // Average number of edges per node
    float init_alpha = ((float)M)/N;

    #if defined(DEBUG)
        auto t1 = std::chrono::high_resolution_clock::now();
    #endif 
    init_embedding(N, M, E, W, Y);
    #if defined(DEBUG)
        auto t2 = std::chrono::high_resolution_clock::now();
        std::cout << "initialize: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
        t1 = std::chrono::high_resolution_clock::now();
    #endif
    optimize(N, M, E, W, Y);
    #if defined(DEBUG)
        t2 = std::chrono::high_resolution_clock::now();
        std::cout << "optimize: "
                << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
                << " ms\n";
    #endif
}
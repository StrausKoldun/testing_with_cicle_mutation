//
// Created by Starus on 01.07.2022.
//

#ifndef SIMULATED_ANNEALING_SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_SIMULATED_ANNEALING_H


#include <functional>
#include <utility>
#include <random>
#include <chrono>

template<class T>
class simulated_annealing{
public:
    simulated_annealing(std::function<long double(long double)> _upd_temp_func,
                        std::function<T(const T &)> _mutate_func,
                        std::function<long double(const T &)> _calc_value_from_T_func,
                        std::function<long double(long double, long double, long double)> _calc_chance_to_mutate);
    T calc_ans(T seed, int count_of_iterations);

private:

    std::function<long double(long double)> upd_temp;
    std::function<T(const T&)> mutate;
    std::function<long double(const T&)> calc_value_from_T;
    std::function<long double(long double, long double, long double)> calc_chance_to_mutate;
    std::mt19937 rnd;
    std::uniform_real_distribution<> dis;
};

template<class T>
simulated_annealing<T>::simulated_annealing(std::function<long double(long double)> _upd_temp_func,
                                            std::function<T(const T &)> _mutate_func,
                                            std::function<long double(const T &)> _calc_value_from_T,
                                            std::function<long double(long double, long double,
                                                                      long double)> _calc_chance_to_mutate)
        :upd_temp(std::move(_upd_temp_func)),
         mutate(std::move(_mutate_func)),
         calc_value_from_T(std::move(_calc_value_from_T)),
         calc_chance_to_mutate(std::move(_calc_chance_to_mutate)),
         rnd(std::chrono::steady_clock::now().time_since_epoch().count()), dis(0.0, 1.0){

}

template<class T>
T simulated_annealing<T>::calc_ans(T seed, int count_of_iterations) {
    long double t=1;
    T ans;
    ans=seed;
    long double ans_value;
    ans_value=calc_value_from_T(seed);
    while(count_of_iterations--){
        t=upd_temp(t);
        auto neighbor=mutate(ans);
        auto neighbor_value=calc_value_from_T(neighbor);
        if(calc_chance_to_mutate(ans_value, neighbor_value, t) >= dis(rnd)){
            std::swap(ans, neighbor);
            std::swap(ans_value, neighbor_value);
        }
    }
    return ans;
}



#endif //SIMULATED_ANNEALING_SIMULATED_ANNEALING_H

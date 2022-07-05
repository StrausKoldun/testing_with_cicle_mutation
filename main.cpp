#include <bits/stdc++.h>
#include "simulated_annealing.h"
using namespace std;

using D = uniform_int_distribution<int>;
void generate_town(vector<vector<double>> &dists);
vector<int> generate_seed(int n);
struct show_progress {
    int count, shag;
    void add(){
        count++;
        if(count%shag==0) {
            cout << "/";
            cout.flush();
        }
    }
};

template<typename Function, typename ...Arguments>
auto carry(Function func, Arguments... args){
    return [=](auto ...rest){
        return func(rest..., args...);
    };
}


int main() {
    mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

    auto swap_mutation = [&rnd](const vector<int> &orig){
        D d(0, orig.size()-1);
        auto copy_mas= orig;
        auto first_number=d(rnd), second_number=d(rnd);

        swap(copy_mas[first_number], copy_mas[second_number]);

        return copy_mas;
    };
    auto insert_mutation=[&rnd](const vector<int> &orig){
        D d(0, orig.size()-1);
        auto copy_mas= orig;
        auto from_pos=d(rnd), to_pos=d(rnd);
        if(from_pos == to_pos) return copy_mas;
        if(from_pos<to_pos){
            while(from_pos!=to_pos){
                swap(copy_mas[from_pos], copy_mas[from_pos+1]);
                from_pos++;
            }
        }
        else{
            while(from_pos!=to_pos){
                swap(copy_mas[from_pos], copy_mas[from_pos-1]);
                from_pos--;
            }
        }
        return copy_mas;
    };
    auto reversal_mutation = [&rnd](const vector<int> &orig){
        D d(0, orig.size()-1);
        auto copy_mas= orig;
        auto from_pos=d(rnd), to_pos=d(rnd);
        if(from_pos>to_pos) swap(from_pos, to_pos);
        reverse(copy_mas.begin()+from_pos, copy_mas.begin()+to_pos);
        return copy_mas;
    };
    auto scramble_mutation = [&rnd](const vector<int> &orig){
        D d(0, orig.size()-1);
        auto copy_mas=orig;
        auto from_pos=d(rnd), to_pos=d(rnd);
        if(from_pos>to_pos) swap(from_pos, to_pos);
        shuffle(copy_mas.begin()+from_pos, copy_mas.begin()+to_pos, rnd);
        return copy_mas;
    };
    auto do_cycle = [](vector<int> &orig, const vector<int> &indexes){
        auto t=orig[indexes[0]];
        for(int i=0; i<indexes.size()-1; i++){
            orig[indexes[i]]=orig[indexes[i+1]];
        }
        orig[indexes.back()]=t;
    };

    auto cycle_mutation_with_maxk=[&rnd, &do_cycle](const vector<int> &orig, int k_max){
        D d(2, k_max);
        vector<int> copy_mas(orig);
        vector<int> all_indexes(copy_mas.size());
        iota(all_indexes.begin(), all_indexes.end(), 0);
        int k = d(rnd);
        vector<int> indexes(k);
        sample(all_indexes.begin(), all_indexes.end(), indexes.begin(), k, rnd);
        do_cycle(copy_mas, indexes);
        return copy_mas;
    };


    auto temp_change = [](long double temp){
        return temp*0.99977;
    };
    vector<vector<double>> dist_btw_points;
    auto calc_distance = [&dist_btw_points](const vector<int> &orig){

        double ans=0;
        for(int i=0; i<orig.size()-1; i++){

            ans+=dist_btw_points[orig[i]][orig[i+1]];
        }
        ans+=dist_btw_points[orig.front()][orig.back()];
        return ans;
    };

    auto chance_to_mutate = [](long double old_state, long double new_state, long double temp){
        if(old_state>new_state) return (long double)1.0;
        return exp((new_state - old_state) / temp);
    };

    using SA=simulated_annealing<vector<int>>;
    SA sa_with_swap(temp_change, swap_mutation, calc_distance, chance_to_mutate);
    SA sa_with_insert(temp_change, insert_mutation, calc_distance, chance_to_mutate);
    SA sa_with_reversal(temp_change, reversal_mutation, calc_distance, chance_to_mutate);
    SA sa_with_scramble(temp_change, scramble_mutation, calc_distance, chance_to_mutate);
    SA sa_with_cycle_5(temp_change, carry(cycle_mutation_with_maxk, 5), calc_distance, chance_to_mutate);
    SA sa_with_cycle_4(temp_change, carry(cycle_mutation_with_maxk, 4), calc_distance, chance_to_mutate);
    SA sa_with_cycle_3(temp_change, carry(cycle_mutation_with_maxk, 3), calc_distance, chance_to_mutate);

    array<pair<double, string>, 7> result{
            pair<double, string>{0, "swap"},
            pair<double, string>{0, "insert"},
            pair<double, string>{0, "reversal"},
            pair<double, string>{0, "scramble"},
            pair<double, string>{0, "cycle5"},
            pair<double, string>{0, "cycle4"},
            pair<double, string>{0, "cycle3"}
    };
    show_progress a{0, 1};
    array<vector<int>, 25> seeds{};
    for(auto &i:seeds){
        i=generate_seed(1000);
    }
    for(int i=0; i<100; i++){
        dist_btw_points.clear();
        dist_btw_points.resize(1000, vector<double>(1000));
        generate_town(dist_btw_points);
        array<double, 7> res{1e9, 1e9, 1e9, 1e9, 1e9, 1e9, 1e9};
        for(auto &seed:seeds){
            res[0]=min(res[0], calc_distance(sa_with_swap.calc_ans(seed, 1000)));
            res[1]=min(res[1], calc_distance(sa_with_insert.calc_ans(seed, 1000)));
            res[2]=min(res[2], calc_distance( sa_with_reversal.calc_ans(seed, 1000)));
            res[3]=min(res[3], calc_distance( sa_with_scramble.calc_ans(seed, 1000)));
            res[4]=min(res[4], calc_distance( sa_with_cycle_3.calc_ans(seed, 1000)));
            res[5]=min(res[5], calc_distance( sa_with_cycle_4.calc_ans(seed, 1000)));
            res[6]=min(res[6], calc_distance( sa_with_cycle_5.calc_ans(seed, 1000)));
        }
        auto min_res= *min_element(res.begin(), res.end());
        int pos=0;
        for(auto j:res){
            result[pos++].first += 2.0-(j/min_res);
        }
        a.add();
    }
    sort(result.begin(), result.end());
    for(auto &i:result){
        cout<<i.first<<" "<<i.second<<endl;
    }

}

vector<int> generate_seed(int n) {
    vector<int> all_indexes(n);
    iota(all_indexes.begin(), all_indexes.end(), 0);
    shuffle(all_indexes.begin(), all_indexes.end(), mt19937(chrono::steady_clock::now().time_since_epoch().count()));

    return all_indexes;
}

void generate_town(vector<vector<double>> &dists) {
    mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());
    D d(0, 100);
    vector<pair<int, int>> cities(dists.size());
    for(auto &city: cities){
        city.first = d(rnd);
        city.second = d(rnd);
    }
    auto calc_dist = [](pair<double, double> a, pair<double, double> b){

        return sqrt((a.first-b.first)*(a.first-b.first) + (a.second-b.second)*(a.second-b.second));
    };
    for(int i=0; i<dists.size(); i++){
        for(int j=0; j<dists.size(); j++){
            dists[i][j]=calc_dist(cities[i], cities[j]);
        }
    }
}

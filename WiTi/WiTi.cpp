#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <numeric>
#include <vector>
#include <algorithm>


struct WiTi {
    int number; // task id
    int p;  // processing time
    int w;  // weight (penalty)
    int d;  // deadline
};


// Load Tasks
std::vector<WiTi> load_tasks(const std::string& filename, const int data_number) {
    std::vector<WiTi> tasks;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "File " << filename << " failed to be opened." << std::endl;
        return tasks;
    }

    std::string s_temp;
    int n;

    while (file >> s_temp && s_temp != "data." + std::to_string(data_number) + ":") {}

    if (s_temp != "data." + std::to_string(data_number) + ":") {
        std::cout << "There is no data." << data_number << " in the file." << std::endl;
    }

    file >> n;

    for (int j = 0; j < n; j++) {
        WiTi task;
        file >> task.p >> task.w >> task.d;
        task.number = j + 1;
        tasks.push_back(task);
    }

    file.close();
    return tasks;
}


int calculate_penalty(int c, int d, int w) {
    return std::max(c - d, 0) * w;
}

std::pair<int, std::vector<int>> PD(std::vector<WiTi>& tasks) {
    int n = tasks.size();

    std::vector<int> memory(1 << n, INT_MAX);
    std::vector<int> PI(1 << n, -1);
    memory[0] = 0;

    // i reprezentuje jakie zadania będą wykonywane
    for (int i = 1; i < (1 << n); i++) { 
        int c = 0;
        for (int j = 0; j < n; ++j) {
            if (i & (1 << j)) {  // Jeżeli w danej kombinacji znajduje się zadanie j to dodawany jest jego czas wykonania
                c += tasks[j].p;
            }
        }

        // Jakie wartości kar można uzyskać, gdy jako ostatnie wykonane będzie zadanie j
        for (int j = 0; j < n; ++j) { // Przejście j po każdym bicie liczby i 
            if (i & (1 << j)) {  
                int subset = i & ~(1 << j); // Sprawdziań najmniejszego kosztu dla zadań bez j w kombinacji ~ 
                int cost = memory[subset] + calculate_penalty(c, tasks[j].d, tasks[j].w);
                if (cost < memory[i]) {
                    memory[i] = cost;
                    PI[i] = j;  // informacja o indeksie zadania, które należy wykonać jako ostatnie
                }
            }
        }
    }

    // Odtwarzanie najlepszej permutacji
    
    std::vector<int> order;
    int current = (1 << n) - 1; // current = 11..1
    
    while (current > 0) {
        int j = PI[current]; // pobieranie indeksu ostatniego do wykonania zadania w optymalnej kombinacji
        order.push_back(tasks[j].number);
        current &= ~(1 << j);
    }
    std::reverse(order.begin(), order.end());

    return { memory[(1 << n) - 1], order };
}


std::pair<int, std::vector<int>> brute_force(std::vector<WiTi>& tasks) {
    int n = tasks.size();

    std::vector<int> best_order;
    int best_cost = INT_MAX;
    std::vector<int> order(n);

    // Generowanie wszystkich możliwych permutacji zadań
    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = i;
    }

    do {
        int current_time = 0;
        int total_penalty = 0;

        for (int i = 0; i < n; ++i) {
            int index = indices[i];
            current_time += tasks[index].p;
            total_penalty += calculate_penalty(current_time, tasks[index].d, tasks[index].w);
        }

        // Jeżeli uzyskaliśmy lepsze rozwiązanie, zapisujemy kolejność zadań i koszt
        if (total_penalty < best_cost) {
            best_order.clear();
            for (int i = 0; i < n; ++i) {
                best_order.push_back(tasks[indices[i]].number);
            }
            best_cost = total_penalty;
        }

    } while (std::next_permutation(indices.begin(), indices.end()));

    return { best_cost, best_order };
}

int main() {

    std::vector<double> pentalties_array;

    
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    std::cout << "                                       PD                                            " << std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    for (int data_number = 10; data_number <= 20; data_number++)
    {

        std::vector<WiTi> data;

        
        data = load_tasks("witi.data.txt", data_number);
        auto start = std::chrono::high_resolution_clock::now();
        auto result1 = PD(data);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Algoritm execution time for dataset "<< data_number << ": " << duration.count() << " microseconds" << std::endl;
        pentalties_array.push_back(result1.first);
        std::cout << "Minimal penalty for dataset " << data_number <<": " << result1.first << std::endl;
        std::cout << "Task permutation: ";
        for (int num : result1.second) {
            std::cout << num << " ";
        }
        std::cout <<  "\n\n";

        
    }

    int total_penalty_sum = std::accumulate(pentalties_array.begin(), pentalties_array.end(), 0);
    std::cout << "Total penalty across 10 datasets: " << total_penalty_sum << std::endl;


    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    std::cout << "                                   BRUTE FORCE                                       " << std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;
    for (int data_number = 10; data_number <= 20; data_number++)
    {
        std::vector<WiTi> data; 
        data = load_tasks("witi.data.txt", data_number);
        auto start_bf = std::chrono::high_resolution_clock::now();
        auto result2 = brute_force(data);
        auto end_bf = std::chrono::high_resolution_clock::now();
        auto duration_bf = std::chrono::duration_cast<std::chrono::microseconds>(end_bf - start_bf);
        std::cout << "Algoritm execution time for dataset " << data_number << ": " << duration_bf.count() << " microseconds" << std::endl;
        pentalties_array.push_back(result2.first);
        std::cout << "Minimal penalty for dataset " << data_number << ": " << result2.first << std::endl;
        std::cout << "Task permutation: ";
        for (int num : result2.second) {
            std::cout << num << " ";
        }
        std::cout << "\n\n";
    }

    return 0;
}

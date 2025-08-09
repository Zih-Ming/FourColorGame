#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <string>

using namespace std;

class Region{
public:
    vector<int> grids;
    char color;
    vector<Region*> neighbors;

    Region(char color = ' '): color(color){}

    void add_grid(int grid_idx){
        grids.push_back(grid_idx);
    }

    void add_neighbor(Region *neighbor){
        neighbors.push_back(neighbor);
    }
};

class Graph{
public:
    int height, width;
    char colors[4] = {'r', 'w', 'b', 'y'};
    vector<Region> regions;
    vector<vector<int>> grids;

    Graph(int h, int w): height(h), width(w){
        grids.resize(h, vector<int>(w));
    }

    void set_size(int h = 0, int w = 0){
        if (h)  grids.resize(h);
        if (w){
            for (vector<int> line: grids){
                line.resize(w);
            }
        }
    }

    void connect(int i, int j){
        regions[i].add_neighbor(&regions[j]);
        regions[j].add_neighbor(&regions[i]);
    }
};


class TerminalGame{
private:
    Graph graph_;
    function<void(double)> generate_graph_impl_;

    // ******************************
    // generate_graph
    // ******************************
    void construct_neighbor_relation(){
        vector<vector<bool>> adjacent(graph_.regions.size(), vector<bool>(graph_.regions.size()));
        for (int i = 0; i < graph_.height - 1; i++){
            for (int j = 0; j < graph_.width; j++){
                int idx1 = graph_.grids[i][j], idx2 = graph_.grids[i+1][j];
                if (idx1 != idx2)   adjacent[idx1][idx2] = adjacent[idx2][idx1] = true;
            }
        }
        for (int i = 0; i < graph_.height; i++){
            for (int j = 0; j < graph_.width - 1; j++){
                int idx1 = graph_.grids[i][j], idx2 = graph_.grids[i][j+1];
                if (idx1 != idx2)   adjacent[idx1][idx2] = adjacent[idx2][idx1] = true;
            }
        }
        for (int i = 0; i < adjacent.size(); i++){
            for (int j = i+1; j < adjacent.size(); j++){
                if (!adjacent[i][j]) continue;
                graph_.connect(i,j);
            }
        }
    }

    void generate_graph_by_removing_edges(double prob){
        random_device rd;
        mt19937 gen(rd());
        bernoulli_distribution d(prob);
        
        vector<vector<bool>> v_edges(graph_.height), h_edges(graph_.height - 1);
        for (auto &edges: v_edges)  for (int i = 0; i < graph_.width - 1; i++)   edges.push_back(d(gen));
        for (auto &edges: h_edges)  for (int i = 0; i < graph_.width; i++)   edges.push_back(d(gen));
        graph_.regions.clear();

        vector<vector<bool>> visited(graph_.height, vector<bool>(graph_.width));
        for (int i = 0; i < graph_.height; i++){
            for (int j = 0; j < graph_.width; j++){
                if (visited[i][j])  continue;
                graph_.regions.push_back(Region());
                dfs_with_random_edges(visited, v_edges, h_edges, i, j);
            }
        }
    }

    void dfs_with_random_edges(vector<vector<bool>> &visited, vector<vector<bool>> &v_edges, vector<vector<bool>> &h_edges, int i, int j){
        if (visited[i][j])  return;
        visited[i][j] = true;
        graph_.regions.back().add_grid(i * graph_.width + j);
        graph_.grids[i][j] = graph_.regions.size() - 1;
        if (i > 0 && !visited[i-1][j] && !h_edges[i-1][j])  dfs_with_random_edges(visited, v_edges, h_edges, i-1, j);
        if (i < graph_.height - 1 && !visited[i+1][j] && !h_edges[i][j]) dfs_with_random_edges(visited, v_edges, h_edges, i+1, j);
        if (j > 0 && !visited[i][j-1] && !v_edges[i][j-1])  dfs_with_random_edges(visited, v_edges, h_edges, i, j-1);
        if (j < graph_.width - 1 && !visited[i][j+1] && !v_edges[i][j])  dfs_with_random_edges(visited, v_edges, h_edges, i, j+1);
    }

    void generate_graph_by_merging_grids(double prob_up, double prob_left){
        random_device rd;
        mt19937 gen(rd());
        bernoulli_distribution d_up(prob_up), d_left(prob_left);
        
        graph_.regions.clear();

        int region_id;
        for (int i = 0; i < graph_.height; i++){
            for (int j = 0; j < graph_.width; j++){
                region_id = graph_.regions.size();
                if (i > 0 && d_up(gen))    region_id = graph_.grids[i-1][j];
                if (j > 0 && d_left(gen))  region_id = graph_.grids[i][j-1];
                graph_.grids[i][j] = region_id;
                if (region_id == graph_.regions.size()){
                    graph_.regions.push_back(Region());
                    graph_.regions.back().add_grid(i * graph_.width + j);
                }
            }
        }
    }

    // ******************************
    // show_graph
    // ******************************
    string determine(int &region_id, int i, int j){
        if (region_id < graph_.regions.size() && graph_.regions[region_id].color != ' ')    region_id++;
        if (region_id != graph_.grids[i][j])    return string(3, graph_.regions[graph_.grids[i][j]].color);
        if (region_id < 10) return " " + to_string(region_id++) + " ";
        if (region_id >= 100)   return to_string(region_id++);
        return " " + to_string(region_id++);
    }
public:
    TerminalGame(int h, int w): graph_(h,w){}
    //     if (generate_by_removing_edges) generate_graph_impl_ = [this](double prob) { generate_graph_by_removing_edges(prob); };
    //     else    generate_graph_impl_ = [this](double prob) { generate_graph_by_merging_grids(prob); };
    // }

    int total_regions(){ return graph_.regions.size(); }

    void set_size(int h = 0, int w = 0){
        graph_.set_size(h, w);
    }

    void generate_graph(double prob){
        generate_graph_by_removing_edges(prob);
        construct_neighbor_relation();
    }

    void generate_graph(double prob1, double prob2){
        generate_graph_by_merging_grids(prob1, prob2);
        construct_neighbor_relation();
    }

    void show_graph(){
        int region_id = 0;
        cout << '*';
        for (int i = 0; i < graph_.width; i++)   cout << " --- *";

        for (int i = 0; i < graph_.height - 1; i++){
            cout << "\n\n|";
            for (int j = 0; j < graph_.width; j++){
                cout << ' ' << determine(region_id, i, j) << ' ' << ((j == graph_.width - 1 || graph_.grids[i][j] != graph_.grids[i][j+1])? '|' : ' '); 
            }
            cout << "\n\n*";
            for (int j = 0; j < graph_.width; j++){
                cout << ' ' <<  ((j == graph_.width - 1 || graph_.grids[i][j] != graph_.grids[i+1][j])? "---" : "   ") << " *"; 
            }
        }

        cout << "\n\n|";
        for (int j = 0; j < graph_.width; j++){
            cout << ' ' << determine(region_id, graph_.height - 1, j) << ' ' << ((j == graph_.width - 1 || graph_.grids[graph_.height - 1][j] != graph_.grids[graph_.height - 1][j+1])? '|' : ' '); 
        }
        cout << "\n\n*";
        for (int i = 0; i < graph_.width; i++)   cout << " --- *";
        cout << endl;
    }

    void show_options(){
        for (int i = 0; i < 4; i++){
            cout << i << ": " << graph_.colors[i] << ", ";
        }
        cout << endl;
    }

    bool draw_graph(int region_id, int color_id = -1){
        if (region_id >= graph_.regions.size()) return false;
        if (color_id < 0){
            graph_.regions[region_id].color = ' ';
            return true;
        }
        char color = graph_.colors[color_id];
        for (Region *region_ptr: graph_.regions[region_id].neighbors){
            if (region_ptr->color == color) return false;
        }
        graph_.regions[region_id].color = color;
        return true;
    }
};


int main(){
    TerminalGame tg(8,16);
    double prob;
    cin >> prob;
    tg.generate_graph(prob);
    tg.show_graph();
    for (int region_id = 0; region_id < tg.total_regions(); region_id++){
        tg.show_options();
        int color_id;
        cin >> color_id;
        if(!tg.draw_graph(region_id, color_id)) region_id--;
        tg.show_graph();
    }
    cout << "\nCongratulations!\n\n" << endl;
    return 0;
}
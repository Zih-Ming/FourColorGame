#include <iostream>
#include <vector>
#include <tuple>
#include <utility>  // forward
#include <random>
#include "definition.hpp"

class Region{
public:
    vector<int> grids;
    char color;

    Region(char color = ' '): color(color){}

    void add_grid(int grid_idx){
        grids.push_back(grid_idx);
    }
};

class Graph{
public:
    int height, width;
    vector<Region> regions;
    vector<vector<int>> grids;

    Graph(int h, int w): height(h), width(w){
        grids.resize(h, vector<int>(w));
    }
};


class TerminalGame: public Game<double, int, int>{
private:
    Graph g;
    void dfs(vector<vector<bool>> &visited, vector<vector<bool>> &v_edges, vector<vector<bool>> &h_edges, int i, int j){
        if (visited[i][j])  return;
        visited[i][j] = true;
        g.regions.back().add_grid(i * g.width + j);
        g.grids[i][j] = g.regions.size() - 1;
        if (i > 0 && !visited[i-1][j] && !h_edges[i-1][j])  dfs(visited, v_edges, h_edges, i-1, j);
        if (i < g.height - 1 && !visited[i+1][j] && !h_edges[i][j]) dfs(visited, v_edges, h_edges, i+1, j);
        if (j > 0 && !visited[i][j-1] && !v_edges[i][j-1])  dfs(visited, v_edges, h_edges, i, j-1);
        if (j < g.width - 1 && !visited[i][j+1] && !v_edges[i][j])  dfs(visited, v_edges, h_edges, i, j+1);
    }
public:
    TerminalGame(int h, int w): g(h,w){}

    void generate_graph(double prob) override{
        random_device rd;
        mt19937 gen(rd());
        bernoulli_distribution d(prob);
        
        vector<vector<bool>> v_edges(g.height), h_edges(g.height - 1);
        for (auto &edges: v_edges)  for (int i = 0; i < g.width - 1; i++)   edges.push_back(d(gen));
        for (auto &edges: h_edges)  for (int i = 0; i < g.width; i++)   edges.push_back(d(gen));
        g.regions.clear();

        vector<vector<bool>> visited(g.height, vector<bool>(g.width));
        for (int i = 0; i < g.height; i++){
            for (int j = 0; j < g.width; j++){
                if (visited[i][j])  continue;
                g.regions.push_back(Region());
                dfs(visited, v_edges, h_edges, i, j);
            }
        }
    }

    virtual void show_graph(int useless = 0) override{
        cout << '*';
        for (int i = 0; i < g.width; i++)   cout << " - *";

        for (int i = 0; i < g.height - 1; i++){
            cout << "\n\n|";
            for (int j = 0; j < g.width; j++){
                cout << ' ' << g.regions[g.grids[i][j]].color << ' ' << ((j == g.width - 1 || g.grids[i][j] != g.grids[i][j+1])? '|' : ' '); 
            }
            cout << "\n\n*";
            for (int j = 0; j < g.width; j++){
                cout << ' ' <<  ((j == g.width - 1 || g.grids[i][j] != g.grids[i+1][j])? '-' : ' ') << " *"; 
            }
        }

        cout << "\n\n|";
        for (int j = 0; j < g.width; j++){
            cout << ' ' << g.regions[g.grids[g.height - 1][j]].color << ' ' << ((j == g.width - 1 || g.grids[g.height - 1][j] != g.grids[g.height - 1][j+1])? '|' : ' '); 
        }
        cout << "\n\n*";
        for (int i = 0; i < g.width; i++)   cout << " - *";
        cout << endl;
    }

    virtual bool draw_graph(int n) override{
        return n != 0;
    }
};

int main(){
    double prob;
    cin >> prob;
    TerminalGame tg(5,10);
    tg.generate_graph(prob);
    tg.show_graph();
    return 0;
}
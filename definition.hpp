using namespace std;

template<typename GraphGenerationConfig, typename ColorConfig>
class Game{
public:
    virtual void generate_graph(GraphGenerationConfig input) = 0;

    virtual void show_graph() = 0;

    virtual bool draw_graph(int region_id, ColorConfig color) = 0;
};
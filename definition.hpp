using namespace std;

template<typename GenerationInput, typename ShowInput, typename DrawInput>
class Game{
public:
    virtual void generate_graph(GenerationInput input) = 0;

    virtual void show_graph(ShowInput input) = 0;

    virtual bool draw_graph(DrawInput input) = 0;
};
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// structs para proibir operações entre valores incompatíveis
struct Position {
	double x;
	Position operator+(Position o) { return {x + o.x}; }
};
struct Speed {
	double x;
	Speed operator+(Speed o) { return {x + o.x}; }
};
struct Accel {
	double x;
	Accel operator+(Accel o) { return {x + o.x}; }
};
struct Mass {
	double x;
	Mass(double x): x(x)
	{
		if (x <= 0) throw std::logic_error("mass can't be negative");
	}
};
struct Force {
	double x;
	Force operator+(Force o) { return {x + o.x}; }
	Accel operator/(Mass o) { return {x / o.x}; }
};
Force force_from_ma(const Mass &m, const Accel &a) { return {m.x * a.x}; }

struct Time {
	double t;
	Time(double t): t(t) {}
	Time operator+(Time o) { return {t + o.t}; }
	Speed operator*(Accel o) { return {t * o.x}; }
	Position operator*(Speed o) { return {t * o.x}; }
};

const Time T{0.01}; // período de amostragem
const Accel g{10.}; // gravidade

class Massa {
	public:
		Mass m;
		Position x;
		Speed xd;

		Massa(double m, double x = 0., double xd = 0.):
			m{m}, x{x}, xd{xd} {}
};

class Mola {
	double k; // constante da mola

	public:
		Mola(double k): k(k) {}
		Force force(Position p) { return {-k * p.x}; }
};

class Amortecedor {
	double b; // constante de amortecimento

	public:
		Amortecedor(double b): b(b) {}
		Force force(Speed s) { return {-b * s.x}; }
};

class View {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture, *bg;
	SDL_Rect target;
	Massa &m;

	int ajustar_coord(const Position &p, int dimensao_objeto, int dimensao_max)
	{
		// 1m = 100px
		return (int)(p.x * 100.) + dimensao_max/2 - dimensao_objeto/2;
	}

	public:
		View(Massa &m):
			window(nullptr), renderer(nullptr),
			texture(nullptr), bg(nullptr), m(m)
		{
			if (SDL_Init (SDL_INIT_VIDEO) < 0) {
				throw std::runtime_error(SDL_GetError());
			}
			window = SDL_CreateWindow("Massa mola",
					SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (window == nullptr) {
				SDL_Quit();
				throw std::runtime_error(SDL_GetError());
			}
			renderer = SDL_CreateRenderer(window, -1,
					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == nullptr) {
				SDL_DestroyWindow(window);
				SDL_Quit();
				throw std::runtime_error(SDL_GetError());
			}

			texture = IMG_LoadTexture(renderer, "../sdl/capi.png");
			bg = IMG_LoadTexture(renderer, "../sdl/park.jpeg");
			SDL_QueryTexture(texture, nullptr, nullptr, &target.w, &target.h);
			Position p{0};
			target.x = ajustar_coord(p, target.w, SCREEN_WIDTH);
			target.y = 0;
		}

		~View()
		{
			SDL_DestroyTexture(texture);
			SDL_DestroyTexture(bg);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void render();

		void delay(Time t) { SDL_Delay(t.t * 1000); }
};

void View::render()
{
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, bg, nullptr, nullptr);
	target.y = ajustar_coord(m.x, target.h, SCREEN_HEIGHT);
	SDL_RenderCopy(renderer, texture, nullptr, &target);

	SDL_RenderPresent(renderer);
}

class Entrada {
	SDL_Event event;
	const Uint8 *keyboard;

	bool m_should_quit;

	public:
		// recebe uma View para garantir que já tenhamos chamado SDL_Init()
		Entrada(const View &v):
			m_should_quit(false)
		{
			keyboard = SDL_GetKeyboardState(nullptr);
		}

		void refresh();
		bool should_quit() { return m_should_quit; }
};

void Entrada::refresh()
{
	SDL_PumpEvents();
	if (keyboard[SDL_SCANCODE_Q]) m_should_quit = true;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			m_should_quit = true;
		}
	}
}

// sistema implementando um modelo massa-mola-amortecedor
// as equações utilizadas para o modelo foram:
// ∑F = mx''
// ∑F = F_elástica + F_amortecedor
// F_elástica = -kx
// F_amortecedor = -bx'
//
// como o sistema pode conter muitas variáveis internas,
// recomenda-se alocá-lo no heap por padrão
class Sistema {
	Massa &massa;
	Mola mola;
	Amortecedor amortecedor;
	Time tempo_total;

	public:
		Sistema(Massa &m, double mola_k, double amortecedor_b):
			massa(m), mola(mola_k), amortecedor(amortecedor_b),
			tempo_total(0) {}

		void simulate(Time t)
		{
			// obter forças para valores atuais
			Force f = mola.force(massa.x);
			f = f + amortecedor.force(massa.xd);
			f = f + force_from_ma(massa.m, g);

			// atualizar posição e velocidade com aceleração atual
			massa.x = massa.x + t*massa.xd;
			massa.xd = massa.xd + t*(f/massa.m);

			// acumular o contador de tempo
			tempo_total = tempo_total + t;
		}
};

int main()
{
	// m(kg), x0(m), v0(m/s)
	Massa m{1, 4, 2};
	// k(N/m), b(kg/s)
	auto sistema = std::make_unique<Sistema>(m, 15, 1.5);

	View v{m};
	Entrada e{v};

	while (!e.should_quit()) {
		v.render();
		sistema->simulate(T);
		e.refresh();
		v.delay(T);
	}
}

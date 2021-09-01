#include <iostream>
#include <memory>

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
struct Time {
	double t;
	Time(double t): t(t) {}
	Time operator+(Time o) { return {t + o.t}; }
	Speed operator*(Accel o) { return {t * o.x}; }
	Position operator*(Speed o) { return {t * o.x}; }
};

const Time T{0.01}; // período de simulação

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
	std::ostream &out;
	public:
		View(std::ostream &out): out(out) {}
		void print(const Massa &m);
		void print(const Time &t);
		void finish_print() { out << std::endl; }
};

void View::print(const Massa &m) { out << m.x.x << "," << m.xd.x; }
void View::print(const Time &t) { out << t.t << ","; }

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
	Massa massa;
	Mola mola;
	Amortecedor amortecedor;
	Time tempo_total;
	View &v;

	public:
		Sistema(double massa_m, double mola_k, double amortecedor_b,
				double x, double xd, View &v):
			massa(massa_m, x, xd), mola(mola_k), amortecedor(amortecedor_b),
			tempo_total(0), v(v) {}

		void simulate(Time t)
		{
			// obter forças para valores atuais
			Force f = mola.force(massa.x);
			f = f + amortecedor.force(massa.xd);

			// atualizar posição e velocidade com aceleração atual
			massa.x = massa.x + t*massa.xd;
			massa.xd = massa.xd + t*(f/massa.m);

			// acumular o contador de tempo
			tempo_total = tempo_total + t;
		}

		void print_fields()
		{
			v.print(tempo_total);
			v.print(massa);
			v.finish_print();
		}
};

int main()
{
	View v{std::cout};
	// m = 10kg
	// k = 3 N/m
	// b = 1.5 kg/s
	// x0 = 5 m
	// v0 = 2 m/s
	auto sistema = std::make_unique<Sistema>(10, 3, 1.5, 5, 2, v);

	for (int i = 0; i < 10000; i++) {
		sistema->print_fields();
		sistema->simulate(T);
	}
}

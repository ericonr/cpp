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
	Mass m;

	public:
		Position x;
		Speed xd;
		Accel xdd;

		Massa(double m, double x = 0., double xd = 0., double xdd = 0.):
			m{m}, x{x}, xd{xd}, xdd{xdd} {}

		void update_accel(Force f) { xdd = f / m; }
		void update_fields(Time t)
		{
			x = x + t * xd;
			xd = xd + t * xdd;
		}

		void print_fields(std::ostream &out)
		{
			out << x.x << "," << xd.x << "," << xdd.x;
		}
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
	std::ostream &out;

	public:
		Sistema(double massa_m, double mola_k, double amortecedor_b,
				double x, double xd, std::ostream &out = std::cout):
			massa(massa_m, x, xd), mola(mola_k), amortecedor(amortecedor_b),
			tempo_total(0), out(out) {}

		void simulate(Time t)
		{
			// obter forças para valores atuais
			Force f = mola.force(massa.x);
			f = f + amortecedor.force(massa.xd);

			// atualizar posição e velocidade com aceleração atual
			massa.update_fields(t);
			// atualizar aceleração baseado na força
			massa.update_accel(f);

			// acumular o contador de tempo
			tempo_total = tempo_total + t;
		}

		void print_fields()
		{
			out << tempo_total.t << ",";
			massa.print_fields(out);
			out << std::endl;
		}
};

int main()
{
	// m = 10kg
	// k = 3 N/m
	// b = 1.5 kg/s
	// x0 = 5 m
	// v0 = 2 m/s
	auto sistema = std::make_unique<Sistema>(10, 3, 1.5, 5, 2);

	for (int i = 0; i < 10000; i++) {
		sistema->print_fields();
		sistema->simulate(T);
	}
}

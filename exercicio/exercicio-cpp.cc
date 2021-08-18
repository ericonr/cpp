#include <iostream>

class Coord {
	int x, y;
	public:
		Coord(int x, int y): x(x), y(y) {}
};

class Cube {
	int x, y, z;
	public:
		Cube(int x, int y, int z): x(x), y(y), z(z) {}

		int volume();
		void print_volume();
};

int Cube::volume()
{
	/* poderia lançar uma exception no caso de overflow */
	return x * y * z;
}

void Cube::print_volume()
{
	std::cout << volume() << std::endl;
}

int main()
{
	auto *c1 = new Coord {50, 10};
	auto *c2 = new Cube {10, 20, 30};

	c2->print_volume();

	delete c1;
	delete c2;

	return 0;
}

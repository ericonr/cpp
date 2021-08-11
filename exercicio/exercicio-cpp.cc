
#include <stdio.h>

typedef struct {
  int x,y;
} coord, *Coord;

typedef struct {
  int x,y,z;
} cube, *Cube;

void coord_new(Coord c, int x, int y) {
  c->x = x;
  c->y = y;
}


void cube_new(Cube c, int x, int y, int z) {
  c->x = x;
  c->y = y;
  c->z = z;
}

void cube_print_volume(Cube c) {
  printf("%d\n", c->x * c->y * c->z);
}

int main() {
  coord c1;
  cube c2;

  coord_new(&c1, 50, 10);
  cube_new(&c2, 10, 20, 30);
  cube_print_volume(&c2);

  return 0;
}

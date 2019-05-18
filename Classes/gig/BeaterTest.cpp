#include "Beater.h"
#include <cmath>
#include <cstdio>

int main()
{
    Beater b(8, 0);

    b.update({0.7607, 1.3057}, 0);
    b.update({1.4638, 2.0353}, 0);
    b.update({2.5649, 2.0486}, 0);
    b.update({3.2415, 3.0170}, 0);
    b.update({3.6196, 4.0344}, 2.75);

    const double max = 6;

    putchar('{');
    for (double i = 0; i < max; i += 0.1) {
        double x = 3.6196 + i;
        double y = b.getY(x);
        printf("(%.4lf, %.4lf)%s",
            x, y,
            i + 0.1 >= max ? "}\n" : ", ");
    }

    putchar('{');
    for (double i = 0; i < max; i += 0.1) {
        double x = 3.6196 + i;
        double y = b.getY(x);
        double k = b.getK(x);
        printf("Vector[(%.4lf, %.4lf), (%.4lf, %.4lf)]%s",
            x, y, x + 1 / sqrt(1 + k * k), y + k / sqrt(1 + k * k),
            i + 0.1 >= max ? "}\n" : ", ");
    }

    return 0;
}

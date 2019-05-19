#include "Beater.h"

using point_t = Beater::point_t;
using line_t = Beater::line_t;

static inline double det3(
    double a11, double a12, double a13,
    double a21, double a22, double a23,
    double a31, double a32, double a33)
{
    return a11 * (a22 * a33 - a23 * a32)
        + a12 * (a23 * a31 - a21 * a33)
        + a13 * (a21 * a32 - a22 * a31);
}

static inline double det4(
    double a11, double a12, double a13, double a14,
    double a21, double a22, double a23, double a24,
    double a31, double a32, double a33, double a34,
    double a41, double a42, double a43, double a44)
{
    return a11 * det3(
        a22, a23, a24,
        a32, a33, a34,
        a42, a43, a44)
    - a12 * det3(
        a21, a23, a24,
        a31, a33, a34,
        a41, a43, a44)
    + a13 * det3(
        a21, a22, a24,
        a31, a32, a34,
        a41, a42, a44)
    - a14 * det3(
        a21, a22, a23,
        a31, a32, a33,
        a41, a42, a43);
}

#define x   first
#define y   second

Beater::line_t Beater::regression() const
{
    double x_bar = 0, y_bar = 0;
    for (const auto &p0 : _q) {
        x_bar += p0.x;
        y_bar += p0.y;
    }
    x_bar /= _q.size();
    y_bar /= _q.size();

    double slr_a_num = 0;
    double slr_a_deno = 0;
    for (const auto &p0 : _q) {
        double xx = p0.x - x_bar;
        double yy = p0.y - y_bar;
        slr_a_num += xx * yy;
        slr_a_deno += xx * xx;
    }
    // TODO: Handle division by zero (though quite unlikely to happen)
    double slr_a = slr_a_num / slr_a_deno;
    double slr_b = y_bar - x_bar * slr_a;

    return line_t(slr_a, slr_b);
}

void Beater::solveCurve(const point_t &p, double k, const line_t &line, double X)
{
    double xp = p.x, yp = p.y;
    double xq = X, yq = line.first * X + line.second;
    double k0 = k, k1 = line.first;

    double D = det4(
        xp * xp * xp, xp * xp, xp, 1,
        xq * xq * xq, xq * xq, xq, 1,
        3 * xp * xp, 2 * xp, 1, 0,
        3 * xq * xq, 2 * xq, 1, 0),
    D3 = det4(
        yp, xp * xp, xp, 1,
        yq, xq * xq, xq, 1,
        k0, 2 * xp, 1, 0,
        k1, 2 * xq, 1, 0),
    D2 = det4(
        xp * xp * xp, yp, xp, 1,
        xq * xq * xq, yq, xq, 1,
        3 * xp * xp, k0, 1, 0,
        3 * xq * xq, k1, 1, 0),
    D1 = det4(
        xp * xp * xp, xp * xp, yp, 1,
        xq * xq * xq, xq * xq, yq, 1,
        3 * xp * xp, 2 * xp, k0, 0,
        3 * xq * xq, 2 * xq, k1, 0),
    D0 = det4(
        xp * xp * xp, xp * xp, xp, yp,
        xq * xq * xq, xq * xq, xq, yq,
        3 * xp * xp, 2 * xp, 1, k0,
        3 * xq * xq, 2 * xq, 1, k1);

    _a = D3 / D;
    _b = D2 / D;
    _c = D1 / D;
    _d = D0 / D;
}

bool Beater::isAscending(double x1, double x2) const
{
    const double EPS = 1e-6;
    double a = 3 * _a, b = 2 * _b, c = _c;
    double x3 = -b / (2 * a);
    return ((a * x1 + b) * x1 + c >= EPS)
        && ((a * x2 + b) * x2 + c >= EPS)
        && (x3 < x1 || x3 > x2 ||
            (a * x3 + b) * x3 + c >= EPS);
}

void Beater::update(const point_t &p, double k)
{
    // Update the window of points
    if (_q.size() >= _size) _q.pop_front();
    _q.push_back(p);
    if (_q.size() < 4) return;

    point_t pp(p.x, this->getY(p.x));

    // Linear regression
    _l = this->regression();

    // Solve for the cubic curve
    double x2_min = p.x + 1;
    solveCurve(pp, k, _l, x2_min);
    if (this->isAscending(p.x, x2_min)) {
        _xFin = x2_min;
        return;
    }

    double x2_max = p.x + 100000;
    // Rare case, so doing a bit heavy work is acceptable
    for (int i = 0; i < 20; i++) {
        double x2 = (x2_min + x2_max) / 2;
        solveCurve(pp, k, _l, x2);
        (this->isAscending(p.x, x2) ? x2_max : x2_min) = x2;
    }
    _xFin = x2_min;
}

#undef x
#undef y

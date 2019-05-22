#include "Beater.h"

#include <cmath>
#include <algorithm>
#include <tuple>

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
    double x_sum = 0, y_sum = 0;
    double xx_sum = 0, yy_sum = 0, xy_sum = 0;
    for (const auto &p0 : _q) {
        x_sum += p0.x;
        y_sum += p0.y;
        xx_sum += p0.x * p0.x;
        yy_sum += p0.y * p0.y;
        xy_sum += p0.x * p0.y;
    }

    double x_bar = x_sum / _q.size();
    double y_bar = y_sum / _q.size();
    double xx_bar = xx_sum / _q.size();
    double yy_bar = yy_sum / _q.size();
    double xy_bar = xy_sum / _q.size();

    // tuple<R, slope, interception>
    std::tuple<double, double, double> best(-1, 0, 0);

    for (int i = -1; i < (int)_q.size(); i++) {
        if (i != -1) {
            // Try removing one sample
            x_bar = (x_sum - _q[i].x) / (_q.size() - 1);
            y_bar = (y_sum - _q[i].y) / (_q.size() - 1);
            xx_bar = (xx_sum - _q[i].x * _q[i].x) / (_q.size() - 1);
            yy_bar = (yy_sum - _q[i].y * _q[i].y) / (_q.size() - 1);
            xy_bar = (xy_sum - _q[i].x * _q[i].y) / (_q.size() - 1);
        }

        double slr_r = (xy_bar - x_bar * y_bar)
            / sqrt((xx_bar - x_bar * x_bar) * (yy_bar - y_bar * y_bar));
        // TODO: Handle division by zero (though quite unlikely to happen)
        double slr_a = (xy_bar - x_bar * y_bar) / (xx_bar - x_bar * x_bar);
        double slr_b = y_bar - x_bar * slr_a;
        best = std::max(best, {slr_r, slr_a, slr_b});
    }

    return line_t(std::get<1>(best), std::get<2>(best));
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

bool Beater::isAscending(double x1, double x2, double min_k) const
{
    const double EPS = 1e-6;
    double a = 3 * _a, b = 2 * _b, c = _c;
    double x3 = -b / (2 * a);
    return ((a * x1 + b) * x1 + c >= min_k + EPS)
        && ((a * x2 + b) * x2 + c >= min_k + EPS)
        && (x3 < x1 || x3 > x2 ||
            (a * x3 + b) * x3 + c >= min_k + EPS);
}

void Beater::update(const point_t &p, double k)
{
    // Update the window of points
    if (_q.size() >= _size) _q.pop_front();
    _q.push_back(p);
    if (_q.size() < 4) return;

    point_t pp(p.x, this->getY(p.x));

    // Linear regression
    auto new_l = this->regression();
    if (new_l.first > 0) _l = new_l;

    // Solve for the cubic curve
    double x2_min = p.x + 1;
    solveCurve(pp, k, _l, x2_min);
    if (this->isAscending(p.x, x2_min, k / 2)) {
        _xFin = x2_min;
        return;
    }

    double x2_max = p.x + 100000;
    // Rare case, so doing a bit heavy work is acceptable
    for (int i = 0; i < 40; i++) {
        double x2 = (x2_min + x2_max) / 2;
        solveCurve(pp, k, _l, x2);
        (this->isAscending(p.x, x2, k / 2) ? x2_max : x2_min) = x2;
    }
    _xFin = x2_max + 1e-3;
}

#undef x
#undef y

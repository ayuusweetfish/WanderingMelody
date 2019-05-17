#ifndef __WanderingMelody__Beater_h__
#define __WanderingMelody__Beater_h__

#include <deque>
#include <utility>

class Beater {
public:
    Beater(int size, double k)
        : _size(size), _a(0), _b(0), _c(k), _d(0) { }
    typedef std::pair<double, double> point_t;
    typedef std::pair<double, double> line_t;

    void update(const point_t &p);
    inline double getY(double x) { return ((_a * x + _b) * x + _c) * x + _d; }
    inline double getK(double x) { return (3 * _a * x + 2 * _b) * x + _c; }

protected:
    const int _size;
    std::deque<point_t> _q;

    double _a, _b, _c, _d;

    inline line_t regression() const;
    inline void solveCurve(const point_t &p, double k, const line_t &line, double x);
};

#endif

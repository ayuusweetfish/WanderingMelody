#ifndef __WanderingMelody__Beater_h__
#define __WanderingMelody__Beater_h__

#include <deque>
#include <utility>

class Beater {
public:
    Beater() { }
    Beater(int size, double k)
        : _size(size), _xFin(0), _l(line_t(k, 0)) { }
    typedef std::pair<double, double> point_t;
    typedef std::pair<double, double> line_t;

    void update(const point_t &p, double k);
    inline double getY(double x) {
        return x < _xFin ?
            ((_a * x + _b) * x + _c) * x + _d :
            _l.first * x + _l.second;
    }
    inline double getK(double x) {
        return x < _xFin ?
            (3 * _a * x + 2 * _b) * x + _c :
            _l.first;
    }

protected:
    int _size;
    std::deque<point_t> _q;

    double _a, _b, _c, _d;
    double _xFin;
    line_t _l;

    inline line_t regression() const;
    inline void solveCurve(const point_t &p, double k, const line_t &line, double x);
    inline bool isAscending(double x1, double x2, double min_k) const;
};

#endif

#ifndef POINT_H
#define POINT_H

/*结构体点*/
template <class T>
struct point {
    inline point() : x(0), y(0) {}
    inline point(T _x, T _y) : x(_x), y(_y) {}
    T x, y;
};

/*点的 + 运算符的重载*/
template <class T>
inline point<T> operator+(const point<T> &p1, const point<T> &p2) {
    return point<T>(p1.x + p2.x, p1.y + p2.y);
}

/*点的 - 运算符的重载*/
template <class T>
inline point<T> operator-(const point<T> &p1, const point<T> &p2) {
    return point<T>(p1.x - p2.x, p1.y - p2.y);
}

/*点的 * 运算符的重载  乘以一个数*/
template <class T>
inline point<T> operator*(const point<T> &p, const T &v) {
    return point<T>(p.x * v, p.y * v);
}

/*点的 * 运算符的重载  乘以一个数*/
template <class T>
inline point<T> operator*(const T &v, const point<T> &p) {
    return point<T>(p.x * v, p.y * v);
}

/*点的 * 运算符的重载  point的内积*/
template <class T>
inline T operator*(const point<T> &p1, const point<T> &p2) {
    return p1.x * p2.x + p1.y * p2.y;
}

#endif // POINT_H

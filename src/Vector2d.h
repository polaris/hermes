#ifndef _Vector2d_H
#define _Vector2d_H

#include <tuple>
#include <cmath>

class Vector2d {
public:
    Vector2d()
    : x_(0)
    , y_(0) {
    }
    
    Vector2d(float x, float y)
    : x_(x)
    , y_(y) {
    }

    Vector2d(const Vector2d& rhs)
    : x_(rhs.x_)
    , y_(rhs.y_) {
    }

    inline float x() const {
        return x_;
    }

    inline float y() const {
        return y_;
    }

    Vector2d operator -() const {
        return Vector2d(-x_, -y_);
    }

    Vector2d& operator =(const Vector2d& rhs) {
        x_ = rhs.x_;
        y_ = rhs.y_;
        return *this;
    }

    Vector2d& operator +=(const Vector2d& v) {
        x_ += v.x_;
        y_ += v.y_;
        return *this;
    }

    Vector2d& operator -=(const Vector2d& v) {
        x_ -= v.x_;
        y_ -= v.y_;
        return *this;
    }
    
    Vector2d& operator *=(const Vector2d& v) {
        x_ *= v.x_;
        y_ *= v.y_;
        return *this;
    }

    Vector2d& operator /=(const Vector2d& v) {
        x_ /= v.x_;
        y_ /= v.y_;
        return *this;
    }

    Vector2d& operator *=(float s) {
        x_ *= s;
        y_ *= s;
        return *this; 
    }

    Vector2d& operator /=(float s) {
        x_ /= s;
        y_ /= s;
        return *this; 
    }

    float length() const {
        return std::sqrt((x_ * x_) + (y_ * y_));
    }

    Vector2d normal() const {
        const auto len = length();
        return Vector2d(x_ / len, y_ / len);
    }

    Vector2d perpendicular() const {
        return Vector2d(y_, -x_);
    }

    friend bool operator ==(const Vector2d& a, const Vector2d& b) {
        return std::tie(a.x_, a.y_) == std::tie(b.x_, b.y_);
    }

    friend bool operator !=(const Vector2d& a, const Vector2d& b) {
        return !(a == b);
    }

private:
    float x_, y_;
};

static Vector2d operator +(const Vector2d& a, const Vector2d& b) {
    return Vector2d(a) += b;
}

static Vector2d operator -(const Vector2d& a, const Vector2d& b) {
    return Vector2d(a) -= b;
}

static Vector2d operator *(const Vector2d& a, const Vector2d& b) {
    return Vector2d(a) *= b;
}

static Vector2d operator /(const Vector2d& a, const Vector2d& b) {
    return Vector2d(a) /= b;
}

static Vector2d operator *(float s, const Vector2d& v) {
    return Vector2d(v) *= s;
}

static Vector2d operator /(float s, const Vector2d& v) {
    return Vector2d(v) /= s;
}

static float dot(const Vector2d& a, const Vector2d& b) {
    return (a.x() * b.x()) + (a.y() * b.y());
}

static float cross(const Vector2d& a, const Vector2d& b) {
    return (a.x() * b.y()) - (a.y() * b.x());
}

static bool intersect(const Vector2d& aa, const Vector2d& ab, const Vector2d& ba, const Vector2d& bb) {
    Vector2d p = aa;
    Vector2d r = ab - aa;
    Vector2d q = ba;
    Vector2d s = bb - ba;

    const auto t = cross((q - p), s) / cross(r, s);
    const auto u = cross((q - p), r) / cross(r, s);

    return (0.0 <= t && t <= 1.0) &&
           (0.0 <= u && u <= 1.0);
}

static Vector2d getIntersect(const Vector2d& aa, const Vector2d& ab, const Vector2d& ba, const Vector2d& bb) {
    const auto pX = (aa.x()*ab.y() - aa.y()*ab.x())*(ba.x() - bb.x()) - (ba.x()*bb.y() - ba.y()*bb.x())*(aa.x() - ab.x());
    const auto pY = (aa.x()*ab.y() - aa.y()*ab.x())*(ba.y() - bb.y()) - (ba.x()*bb.y() - ba.y()*bb.x())*(aa.y() - ab.y());
    
    const auto denominator = (aa.x() - ab.x())*(ba.y() - bb.y()) - (aa.y() - ab.y())*(ba.x() - bb.x());
    
    return Vector2d(pX / denominator, pY / denominator);    
}

#endif  // _Vector2d_H

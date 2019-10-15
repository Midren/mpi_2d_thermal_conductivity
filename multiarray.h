#ifndef MPI_2D_THERMAL_CONDUCTIVITY_MYVECTOR_H
#define MPI_2D_THERMAL_CONDUCTIVITY_MYVECTOR_H


class multiArray {
public:

    explicit multiArray(size_t height, size_t width) : h(height), w(width) {
        elems = static_cast<double *>(::operator new(height * width * sizeof(double)));
    }

    ~multiArray() {
        ::operator delete(elems);
    }

    multiArray(const multiArray &arr) : h(arr.h), w(arr.w) {
        elems = static_cast<double *>(::operator new(arr.h * arr.w * sizeof(double)));
        memcpy(elems, arr.elems, arr.h * arr.w * sizeof(double));
    }

    multiArray &operator=(const multiArray &arr) {
        if (h * w < arr.h * arr.w) {
            ::operator delete(elems);
            elems = static_cast<double *>(::operator new(arr.h * arr.w * sizeof(double)));
        }
        memcpy(elems, arr.elems, arr.h * arr.w * sizeof(double));
        return *this;
    }

    multiArray &operator=(multiArray &&arr) noexcept {
        this->elems = arr.elems;
        arr.elems = nullptr;
        return *this;
    }

    bool swap(multiArray &arr) {
        if (arr.h != h || arr.w != w)
            return true;
        std::swap(elems, arr.elems);
        return false;
    }

    inline double &operator()(size_t x, size_t y) {
        return elems[y * w + x];
    }

    inline double operator()(size_t x, size_t y) const {
        return elems[y * w + x];
    }

    inline size_t height() const {
        return h;
    }

    inline size_t width() const {
        return w;
    }

    double *data() {
        return elems;
    }

private:
    double *elems;
    size_t h;
    size_t w;
};

#endif //MPI_2D_THERMAL_CONDUCTIVITY_MYVECTOR_H

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

    void print(std::string path) {
        std::ofstream fout(path);
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++)
                fout << j << " " << i << " " << this->operator()(i, j) << std::endl;
            fout << std::endl;
        }
    }

    bool swap(multiArray &arr) {
        if (arr.h != h || arr.w != w)
            return true;
        std::swap(elems, arr.elems);
        return false;
    }

    void set_row(const size_t &row_height, const double *array) {
        memcpy(elems + row_height * w, array, w * sizeof(double));
    }

    double *get_row(const size_t &row_height) {
        return elems + row_height * w;
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

    std::vector<std::vector<double >> to_2d() {
        std::vector<std::vector<double >> rows;
        for (int i = 0; i < h; i++) {
            double *tmp = get_row(i);
            std::vector<double> row;
            for (int j = 0; j < w; j++) {
                row.push_back(tmp[i]);
            }
            rows.push_back(row);
        }
        return rows;
    }

private:
    double *elems;
    size_t h;
    size_t w;
};

#endif //MPI_2D_THERMAL_CONDUCTIVITY_MYVECTOR_H

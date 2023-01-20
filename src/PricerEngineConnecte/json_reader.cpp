#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json_reader.hpp"

void from_json(const nlohmann::json &j, PnlVect *&vect) {
    std::vector<double> stl_v = j.get<std::vector<double>>();
    vect = pnl_vect_create_from_ptr(stl_v.size(), stl_v.data());
}

void from_json(const nlohmann::json &j, PnlMat *&mat) {
    std::vector<std::vector<double>> stl_m = j.get<std::vector<std::vector<double>>>();
    int nLines = stl_m.size();
    if (nLines == 0) {
        mat = pnl_mat_create(0, 0);
        return;
    }
    int nColumns = stl_m[0].size();
    for (auto it : stl_m) {
        if (nColumns != it.size()) {
            std::cerr << "Matrix is not regular" << std::endl;
            mat = NULL;
            return;
        }
    }
    mat = pnl_mat_create(nLines, nColumns);
    int m = 0;
    for (auto row : stl_m) {
        pnl_mat_set_row_from_ptr(mat, row.data(), m);
        m++;
    }
}

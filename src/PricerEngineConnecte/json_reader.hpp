#pragma once

#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

void from_json(const nlohmann::json &j, PnlVect *&vect);
void from_json(const nlohmann::json &j, PnlMat *&mat);

#include "ceres/ceres.h"
#include "utils/print.h"

namespace {

constexpr int kNumObservations = 67;
// clang-format off
constexpr double data[] = {
  0.000000e+00, 1.133898e+00,
  7.500000e-02, 1.334902e+00,
  1.500000e-01, 1.213546e+00,
  2.250000e-01, 1.252016e+00,
  3.000000e-01, 1.392265e+00,
  3.750000e-01, 1.314458e+00,
  4.500000e-01, 1.472541e+00,
  5.250000e-01, 1.536218e+00,
  6.000000e-01, 1.355679e+00,
  6.750000e-01, 1.463566e+00,
  7.500000e-01, 1.490201e+00,
  8.250000e-01, 1.658699e+00,
  9.000000e-01, 1.067574e+00,
  9.750000e-01, 1.464629e+00,
  1.050000e+00, 1.402653e+00,
  1.125000e+00, 1.713141e+00,
  1.200000e+00, 1.527021e+00,
  1.275000e+00, 1.702632e+00,
  1.350000e+00, 1.423899e+00,
  1.425000e+00, 1.543078e+00,
  1.500000e+00, 1.664015e+00,
  1.575000e+00, 1.732484e+00,
  1.650000e+00, 1.543296e+00,
  1.725000e+00, 1.959523e+00,
  1.800000e+00, 1.685132e+00,
  1.875000e+00, 1.951791e+00,
  1.950000e+00, 2.095346e+00,
  2.025000e+00, 2.361460e+00,
  2.100000e+00, 2.169119e+00,
  2.175000e+00, 2.061745e+00,
  2.250000e+00, 2.178641e+00,
  2.325000e+00, 2.104346e+00,
  2.400000e+00, 2.584470e+00,
  2.475000e+00, 1.914158e+00,
  2.550000e+00, 2.368375e+00,
  2.625000e+00, 2.686125e+00,
  2.700000e+00, 2.712395e+00,
  2.775000e+00, 2.499511e+00,
  2.850000e+00, 2.558897e+00,
  2.925000e+00, 2.309154e+00,
  3.000000e+00, 2.869503e+00,
  3.075000e+00, 3.116645e+00,
  3.150000e+00, 3.094907e+00,
  3.225000e+00, 2.471759e+00,
  3.300000e+00, 3.017131e+00,
  3.375000e+00, 3.232381e+00,
  3.450000e+00, 2.944596e+00,
  3.525000e+00, 3.385343e+00,
  3.600000e+00, 3.199826e+00,
  3.675000e+00, 3.423039e+00,
  3.750000e+00, 3.621552e+00,
  3.825000e+00, 3.559255e+00,
  3.900000e+00, 3.530713e+00,
  3.975000e+00, 3.561766e+00,
  4.050000e+00, 3.544574e+00,
  4.125000e+00, 3.867945e+00,
  4.200000e+00, 4.049776e+00,
  4.275000e+00, 3.885601e+00,
  4.350000e+00, 4.110505e+00,
  4.425000e+00, 4.345320e+00,
  4.500000e+00, 4.161241e+00,
  4.575000e+00, 4.363407e+00,
  4.650000e+00, 4.161576e+00,
  4.725000e+00, 4.619728e+00,
  4.800000e+00, 4.737410e+00,
  4.875000e+00, 4.727863e+00,
  4.950000e+00, 4.669206e+00,
};
// clang-format on

class AnalyticExpCost : public ceres::SizedCostFunction<1, 1, 1> {
public:
    AnalyticExpCost(double x, double y) : x_(x), y_(y) {}
    virtual ~AnalyticExpCost() {}
    bool Evaluate(double const* const* parameters, double* residuals, double** jacobians) const override {
        const double m = parameters[0][0];
        const double c = parameters[1][0];

        const double curr_y = std::exp(m * x_ + c);
        residuals[0] = y_ - curr_y;

        if (jacobians) {
            if (jacobians[0]) jacobians[0][0] = -x_ * curr_y;
            if (jacobians[1]) jacobians[1][0] = -curr_y;
        }
        return true;
    }

private:
    double x_;  // x_的观测
    double y_;  // y_的观测
};

void curve_fitting() {
    double m = 0.0, c = 0.0;
    ceres::Problem problem;
    ceres::Solver::Summary summary;
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    for (int i = 0; i < kNumObservations; ++i) {
        problem.AddResidualBlock(new AnalyticExpCost(data[2 * i], data[2 * i + 1]), nullptr, &m, &c);
    }
    ceres::Solve(options, &problem, &summary);
    utils::print(summary.BriefReport());
    utils::print("m:", m, ", c:", c);
}

void curve_fitting_robust() {
    double m = 0.0, c = 0.0;
    ceres::Problem problem;
    ceres::Solver::Summary summary;
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    for (int i = 0; i < kNumObservations; ++i) {
        problem.AddResidualBlock(new AnalyticExpCost(data[2 * i], data[2 * i + 1]), new ceres::CauchyLoss(0.5), &m, &c);
    }
    ceres::Solve(options, &problem, &summary);
    utils::print(summary.BriefReport());
    utils::print("m:", m, ", c:", c);
}

}  // namespace

int main() {
    curve_fitting();
    utils::print("----------------------------------");
    curve_fitting_robust();
}
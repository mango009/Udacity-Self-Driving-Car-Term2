#include <math.h>
#include <iostream>
#include "kalman_filter.h"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in_l, MatrixXd &R_in_r, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_l = R_in_l;  
  R_r = R_in_r;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_; 
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_l;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;  
}

void KalmanFilter::RadarHofX(VectorXd& v) {
   float rho = sqrt(x_[0]*x_[0]+x_[1]*x_[1]);
   float phi = atan2(x_[1], x_[0]);
   float rho_dot = (x_[0]*x_[2]+x_[1]*x_[3])/rho;
    v << rho, phi, rho_dot;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
  Tools tools;

  VectorXd z_pred = VectorXd(3);
  RadarHofX(z_pred);
  VectorXd y = z - z_pred;

  // normalize the angle so that the value is between -pi and pi
  while (y(1)> M_PI) y(1)-=2.*M_PI;
  while (y(1)<-M_PI) y(1)+=2.*M_PI;
  

  MatrixXd Hj = tools.CalculateJacobian(x_);
  MatrixXd Ht = Hj.transpose();
  MatrixXd S = Hj * P_ * Ht + R_r;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * Hj) * P_;  
}


#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);
  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
        0, 0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0, 0,
        0, 0.0009, 0,
        0, 0, 0.09;

  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */
  MatrixXd F = MatrixXd(4, 4);
  F << 1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1;
  MatrixXd H = MatrixXd(2, 4);
  H << 1, 0, 0, 0,
        0, 1, 0, 0; 

  //state covariance matrix P
  MatrixXd P = MatrixXd(4, 4);
  P << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1000, 0,
        0, 0, 0, 1000;               

  VectorXd x = VectorXd(4);
  MatrixXd Q = MatrixXd(4,4);
  // x will be assigned on the first call to ProcessMeasurement
  // Q will be  computed on every call to ProcessMeasurement
  ekf_.Init(x, P, F, H, R_laser_, R_radar_, Q);
}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
    TODO:
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
    // first measurement
    cout << "EKF: " << endl;
    ekf_.x_ = VectorXd(4);
    //ekf_.x_ << 1, 1, 1, 1;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
      ekf_.x_ << measurement_pack.raw_measurements_[0]*cos(measurement_pack.raw_measurements_[1]),  
        measurement_pack.raw_measurements_[0]*sin(measurement_pack.raw_measurements_[1]), 1, 1;
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state.
      */
      ekf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 1, 1;
    }

    previous_timestamp_ = measurement_pack.timestamp_;

    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
   TODO:
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
     * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */
  static float noise_ax = 9;
  static float noise_ay = 9;
  //compute the time elapsed between the current and previous measurements
  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0; //dt - expressed in seconds
  previous_timestamp_ = measurement_pack.timestamp_;

  ekf_.F_(0, 2) = dt;
  ekf_.F_(1,3) = dt;

  //2. Set the process covariance matrix Q
  MatrixXd Q = MatrixXd(4,4);
  float dt2 = dt*dt;
  float dt3 = dt2*dt; 
  float dt4 = dt3*dt; 
  float ax2 = noise_ax;
  float ay2 = noise_ay;
  Q << (dt4/4.0*ax2), 0, (dt3/2.0*ax2), 0,
      0, (dt4/4.0*ay2), 0, (dt3/2.0*ay2),
      (dt3/2.0*ax2), 0, (dt2*ax2), 0,
      0, (dt3/2.0*ay2), 0, (dt2*ay2);
  ekf_.Q_ = Q;

  ekf_.Predict();

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
   TODO:
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    float px = ekf_.x_(0);
    float py = ekf_.x_(1);
    if(fabs(px*px+py*py) < 0.0001) {
      ekf_.UpdateEKF(measurement_pack.raw_measurements_);
    } else {
      cout << "Avoiding Division by Zero, skip update step" << endl;
    }
  } else {
    // Laser updates
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}

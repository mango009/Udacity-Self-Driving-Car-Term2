#include <iostream>
#include "PID.h"

/*
* TODO: Complete the PID class.
*/

PID::PID() {
	prev_cte = 0.0;
}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
	this->Kp = Kp;
	this->Ki = Ki;
	this->Kd = Kd;
	p_error =  i_error = d_error = 0;
}

void PID::UpdateError(double cte) {

	p_error = cte;
	d_error = cte - prev_cte;
	i_error +=  cte;

	prev_cte = cte;
}

double PID::TotalError() {
	return Kp*p_error + Kd*d_error + Ki*i_error;
}


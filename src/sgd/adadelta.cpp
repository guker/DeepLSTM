#include <math.h>
#include <string.h>
#include "sgd.h"

adadelta::adadelta (boost::property_tree::ptree *confReader, string section, int paramSize) : sgdBase(confReader, section, paramSize){
	m_decayFactor = confReader->get<float>(section + "adadelta_decay_factor");
	m_stableConst = confReader->get<float>(section + "adadelta_stable_const");

	m_ESquareGrad  = new float [m_paramSize];
	m_ESquareDelta = new float [m_paramSize];

	memset(m_ESquareGrad, 0x00, sizeof(float) * m_paramSize);
	memset(m_ESquareDelta, 0x00, sizeof(float) * m_paramSize);
}

adadelta::~adadelta () {
	if (m_ESquareGrad != NULL) {
		delete [] m_ESquareGrad;
	}
	if (m_ESquareDelta != NULL) {
		delete [] m_ESquareDelta;
	}
}

void adadelta::updateParams (float *params, float *grad, int rank) {
	float delta;
	for (int i=0; i<m_paramSize; i++) {
		// accumulate mean squared grad
		m_ESquareGrad[i] = m_decayFactor * m_ESquareGrad[i] + (1 - m_decayFactor) * grad[i] * grad[i];
		// compute delta
		delta = sqrt(m_ESquareDelta[i] + m_stableConst) / sqrt(m_ESquareGrad[i] + m_stableConst) * grad[i];
		m_velocity[i] = m_momentumFactor * m_velocity[i] - delta;
		params[i] += m_velocity[i];
		// accumulate mean squared delta
		m_ESquareDelta[i] = m_decayFactor * m_ESquareDelta[i] + (1 - m_decayFactor) * delta * delta;
	}
}
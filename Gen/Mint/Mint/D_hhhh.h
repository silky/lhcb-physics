/*
 * MintGen.h
 *
 *  Created on: May 11, 2011
 *      Author: mcoombes
 *
 *      Purpose: Interface between Gauss model and MINT
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

//C++
#include <string>
#include <iostream>
#include <vector>

//MINT
#include "Mint/SignalGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IMintGen.h"

//ROOT
#include "TVector3.h"

namespace MINT {
	class MintGen : virtual public IMintGen{
	public:
		MintGen();
		virtual ~MintGen();

		void Initalize(std::vector<int>);

		void SetInputTextFile(std::string inputFile);

		// Decay Event in parent Rest Frame
		std::vector<std::vector<double> > DecayEventRFVec();

		std::vector<double> getDaugtherMom(IDalitzEvent* , int );

		//DalitzEvent
		void SetDalitzEvent(IDalitzEvent*);
	
	private:
		std::string m_inputFileName;
		IDalitzEvent* m_dE;
		SignalGenerator* m_sg;
	};
}

#endif /* MintGen_H_ */

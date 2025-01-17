
// #pragma GCC optimize ("O0")

#include "observations.hpp"
#include "instrument.hpp"
#include "GNSSambres.hpp"
#include "testUtils.hpp"
#include "acsConfig.hpp"
#include "constants.hpp"
#include "station.hpp"
#include "satStat.hpp"
#include "trace.hpp"
#include "sinex.hpp"
#include "acsQC.hpp"
#include "ppp.hpp"


void outputObservations(
	Trace&		trace,
	ObsList&	obsList)
{
	for (auto& obs : only<GObs>(obsList))
// 	for (auto& [ft, sig] : obs.Sigs)
	for (auto& [ft, sigs] : obs.SigsLists)
	for (auto& sig : sigs)
	{
		if (obs.exclude)
		{
			continue;
		}
		
		auto& satStat = *obs.satStat_ptr;
		
		tracepdeex(4, trace, "\n%s %5s %5s %14.4f %14.4f", obs.time.to_string(2).c_str(), obs.Sat.id().c_str(), sig.code._to_string(), sig.L, sig.P);
	}
}



void obsVariances(
	ObsList& obsList)
{
	for (auto& obs			: only<GObs>(obsList))
	if (obs.satNav_ptr)
	if (obs.satStat_ptr)
	if (acsConfig.process_sys[obs.Sat.sys])
	{
		auto& recOpts = acsConfig.getRecOpts(obs.mount);
		auto& satOpts = acsConfig.getSatOpts(obs.Sat); 
		
		double el = obs.satStat_ptr->el;
		if (el == 0)
			el = PI/8;

		double elevationScaling = 1;
		switch (recOpts.error_model)
		{
			case E_NoiseModel::UNIFORM:					{	elevationScaling = 1;				break;	}
			case E_NoiseModel::ELEVATION_DEPENDENT:		{	elevationScaling = 1 / sin(el);		break;	} 
		}

		auto freqSigma = [elevationScaling](int ft, vector<double>& sigmasVec)
		{
			//get the sigma for this frequency, (or the last one in the list)	
			if (ft >= sigmasVec.size())		
				ft =  sigmasVec.size() - 1;

			double sigmaCode = sigmasVec[ft];

			sigmaCode *= elevationScaling;

			return SQR(sigmaCode);
		};
		
		for (auto& [ft, Sig]	: obs.Sigs)
		{
			Sig.codeVar = 0;
			Sig.phasVar = 0;
			
			Sig.codeVar += freqSigma(ft, recOpts.code_sigmas);			Sig.codeVar += freqSigma(ft, satOpts.code_sigmas);	
			Sig.phasVar += freqSigma(ft, recOpts.phas_sigmas);			Sig.phasVar += freqSigma(ft, satOpts.phas_sigmas);
		}
		
		for (auto& [ft, sigList] : obs.SigsLists)
		for (auto& Sig : sigList)
		{
			Sig.codeVar = 0;
			Sig.phasVar = 0;
			
			Sig.codeVar += freqSigma(ft, recOpts.code_sigmas);			Sig.codeVar += freqSigma(ft, satOpts.code_sigmas);	
			Sig.phasVar += freqSigma(ft, recOpts.phas_sigmas);			Sig.phasVar += freqSigma(ft, satOpts.phas_sigmas);
		}
	}
}

void excludeUnprocessed(
	ObsList&	obsList)
{
	for (auto& obs : only<GObs>(obsList))
	{
		if (acsConfig.process_sys[obs.Sat.sys] == false)
		{
			obs.excludeSystem = true;
		}
	}
}

void recordSlips(
	Station&	rec)
{
	for (auto& obs			: only<GObs>(rec.obsList))
	for (auto& [ft, sig]	: obs.Sigs)
	if  (obs.satStat_ptr)
	{
		SigStat& sigStat = obs.satStat_ptr->sigStatMap[ft2string(ft)];
		
		if	(	sigStat.slip.any
			&&( (acsConfig.excludeSlip.LLI		&& sigStat.slip.LLI)
			  ||(acsConfig.excludeSlip.GF		&& sigStat.slip.GF)	
			  ||(acsConfig.excludeSlip.MW		&& sigStat.slip.MW)	
			  ||(acsConfig.excludeSlip.SCDIA	&& sigStat.slip.SCDIA)))
		{
			rec.savedSlips[obs.Sat] = obs.time;
		}
	}
}

void preprocessor(
	Network&	net,
	Station&	rec)
{
	if (acsConfig.process_preprocessor == false)
	{
		return;
	}
	
	Instrument instrument(__FUNCTION__);
	
	auto trace = getTraceFile(rec);
		
	acsConfig.getRecOpts(rec.id);
	
	auto& obsList = rec.obsList;
	
	rec.sol.time = obsList.front()->time;
	
	PTime start_time;
	start_time.bigTime = boost::posix_time::to_time_t(acsConfig.start_epoch);

	double tol;
	if (acsConfig.assign_closest_epoch)			tol = acsConfig.epoch_interval / 2;
	else										tol = 0.5;
	
	if	(  acsConfig.start_epoch.is_not_a_date_time() == false
		&& rec.sol.time < (GTime) start_time - tol)
	{
		return;
	}

	//prepare and connect navigation objects to the observations
	for (auto& obs : only<GObs>(obsList))
	{
		obs.mount = rec.id;
		
		if (acsConfig.process_sys[obs.Sat.sys] == false)
		{
			continue;
		}
		
		obs.satNav_ptr = &nav.satNavMap[obs.Sat];
		
		E_NavMsgType nvtyp = acsConfig.used_nav_types[obs.Sat.sys];
		if (obs.Sat.sys == +E_Sys::GLO)		obs.satNav_ptr->eph_ptr = seleph<Geph>	(trace, obs.time, obs.Sat, nvtyp, ANY_IODE, nav);
		else								obs.satNav_ptr->eph_ptr = seleph<Eph>	(trace, obs.time, obs.Sat, nvtyp, ANY_IODE, nav);
		
		updatenav(obs);

		obs.satStat_ptr = &rec.satStatMap[obs.Sat];

		acsConfig.getSatOpts(obs.Sat);
		
		//ar stuff
		{
			if (acsConfig.process_network)	ARstations["NETWORK"].ID	= "NETWORK";
			else							ARstations[rec.id].ID		= rec.id;
			
			sys_activ[rec.id];
		
			ARsatellites[obs.Sat];
			
			for (E_AmbTyp ambType : E_AmbTyp::_values())
			{
				elev_archive[{KF::AMBIGUITY, obs.Sat, obs.mount, ambType}];
				slip_archive[{KF::AMBIGUITY, obs.Sat, obs.mount, ambType}];
			}
		} 
	}

	for (auto& obs : only<LObs>(obsList))
	{
		//obs.mount = rec.id;
		
		if (acsConfig.process_sys[obs.Sat.sys] == false)
		{
			continue;
		}
		
		obs.satNav_ptr = &nav.satNavMap[obs.Sat];
		
		E_NavMsgType nvtyp = acsConfig.used_nav_types[obs.Sat.sys];
		if (obs.Sat.sys == +E_Sys::GLO)		obs.satNav_ptr->eph_ptr	= seleph<Geph>	(trace, obs.time, obs.Sat, nvtyp, ANY_IODE, nav);
		else								obs.satNav_ptr->eph_ptr	= seleph<Eph>	(trace, obs.time, obs.Sat, nvtyp, ANY_IODE, nav);
		
		updatenav(obs);

		obs.satStat_ptr = &rec.satStatMap[obs.Sat];
	}
	
	clearSlips(obsList);
	
	excludeUnprocessed(obsList);
	
	outputObservations(trace, obsList);
	
	for (auto& obs : only<GObs>(obsList))
		satPosClk(trace, rec.sol.time, obs, nav, acsConfig.model.sat_pos.ephemeris_sources, acsConfig.model.sat_clock.ephemeris_sources, nullptr, E_OffsetType::APC);
	
	obsVariances(obsList);

	/* linear combinations */
	for (auto& obs : only<GObs>(obsList))
	if (obs.satStat_ptr)
	{
		obs.satStat_ptr->lc_pre = obs.satStat_ptr->lc_new;
		obs.satStat_ptr->lc_new = {};
	}
	obs2lcs		(trace,	obsList);

	detectslips	(trace,	obsList);

	recordSlips(rec);
	
	for (auto& obs			: only<GObs>(obsList))
	for (auto& [ft, Sig]	: obs.Sigs)
	if  (obs.satStat_ptr)
	{
		if (obs.satStat_ptr->sigStatMap[ft2string(ft)].slip.any)
		{
			rec.slipCount++;
			break;
		}
	}
}


// #pragma GCC optimize ("O0")

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <chrono>

using std::chrono::system_clock;
using std::chrono::time_point;

using bsoncxx::builder::basic::kvp;

#include "ntripBroadcast.hpp"
#include "mongoRead.hpp"
#include "otherSSR.hpp"
#include "fileLog.hpp"
#include "gTime.hpp"


NtripBroadcaster ntripBroadcaster;

namespace bp = boost::asio::placeholders;


using boost::posix_time::ptime;
using boost::posix_time::time_duration;

void debugSSR(GTime t0, GTime targetTime, E_Sys sys, SsrOutMap& ssrOutMap);

void NtripBroadcaster::startBroadcast()
{
	NtripSocket::startClients();
}

void NtripBroadcaster::stopBroadcast()
{
	for (auto [label, outStream] : ntripUploadStreams)
	{
		outStream->disconnect();
	}
	
	ntripUploadStreams.clear();
}

void NtripUploader::serverResponse(
	unsigned int	status_code,
	string 			http_version)
{
	if (acsConfig.output_ntrip_log == false)
		return;

	std::ofstream logStream(networkTraceFilename, std::ofstream::app);
	
	if (!logStream)
	{
		BOOST_LOG_TRIVIAL(warning) << "Error opening log file.\n";
		return;
	}

	GTime time = timeGet();

	bsoncxx::builder::basic::document doc = {};
	doc.append(kvp("label", 			"serverResponse"));
	doc.append(kvp("Stream", 			url.path.substr(1,url.path.length())));
	doc.append(kvp("Time", 				time.to_string()));
	doc.append(kvp("ServerStatus", 		(int)status_code));
	doc.append(kvp("VersionHTTP",		http_version));
	
	logStream << bsoncxx::to_json(doc) << std::endl;
}


void NtripUploader::write_handler(
	const boost::system::error_code& err)
{
	if (err)
	{
		outMessages.consume(outMessages.size());
		outMessagesMtx.unlock();
		
		ERROR_OUTPUT_RECONNECT_AND_RETURN;
	}
	
	onChunkSentStatistics();
	
	outMessagesMtx.unlock();
}

void NtripUploader::messageTimeout_handler(
	const boost::system::error_code& err)
{
// 	BOOST_LOG_TRIVIAL(debug) << "started " << __FUNCTION__ << "\n";
	if (err)
	{
		ERROR_OUTPUT_RECONNECT_AND_RETURN;
	}

	//fire this callback again in the future
	{
		sendTimer.expires_from_now(boost::posix_time::milliseconds(500));		// check uploader twice a second to account for aliasing
		sendTimer.async_wait(boost::bind(&NtripUploader::messageTimeout_handler, this, bp::error));
	}
	
	SSRMeta		ssrMeta;
	SsrOutMap	ssrOutMap;
	
	GTime	latestTime;
	
	if		(acsConfig.ssrOpts.output_timing == +E_SSROutTiming::GPS_TIME) 					latestTime = timeGet();
	else if (acsConfig.ssrOpts.output_timing == +E_SSROutTiming::LATEST_CLOCK_ESTIMATE)		latestTime = mongoReadLastClock();
	
	if (latestTime == GTime::noTime())
		return;
	
	GTime	targetTime	= latestTime.floorTime(1);
	
	if (targetTime == previousTargetTime)
	{
		//already did this epoch
		return;
	}
	
	BOOST_LOG_TRIVIAL(debug) << "SSR OUT Targeting epoch: " << targetTime.to_string(0) << std::endl;
	
	ssrMeta.receivedTime		= targetTime;	// for rtcmTrace (debugging)
	ssrMeta.multipleMessage 	= 1; // We assume there will be more messages.
	
	if (streamConfig.itrf_datum)		ssrMeta.referenceDatum 	=  0; // Orbit corrections, 0 - ITRF, 1 - Regional.
	else								ssrMeta.referenceDatum 	=  1;
	
	ssrMeta.provider			= streamConfig.provider_id;
	ssrMeta.solution			= streamConfig.solution_id;

	for (auto [messCode, msgOpts] : streamConfig.rtcmMsgOptsMap)
	{
		int updateInterval = msgOpts.udi;

		if	( updateInterval == 0
			||((long int)targetTime) % updateInterval != 0)
		{
			continue;
		}
		
		int udiIndex = getUdiIndex(updateInterval);
		
		ssrMeta.updateIntIndex	= udiIndex;
		
		if (ssrMeta.updateIntIndex == -1)
			BOOST_LOG_TRIVIAL(error) << "Error: ssrMeta.updateIntIndex is not valid :" << ssrMeta.updateIntIndex << ").";

		if (messCode == streamConfig.rtcmMsgOptsMap.rbegin()->first)
			ssrMeta.multipleMessage = 0;
		
		E_Sys sys = rtcmMessageSystemMap[messCode];
		
		if (sys == +E_Sys::NONE)
		{
			BOOST_LOG_TRIVIAL(error) << "Error: invalid message code system :" << messCode;
			continue;
		}
		
		if		(sys == +E_Sys::GLO)	{	RTod tod = targetTime;			ssrMeta.epochTime1s		= (int)tod;		}	
		else if	(sys == +E_Sys::BDS)	{	BTow tow = targetTime;			ssrMeta.epochTime1s		= (int)tow;		}
		else							{	GTow tow = targetTime;			ssrMeta.epochTime1s		= (int)tow;		}
		
		GTime t0;
		if (ssrMeta.updateIntIndex == 0)	t0 = targetTime;
		else								t0 = targetTime + updateInterval / 2.0;

		switch (messCode)
		{
			case +RtcmMessageType::GPS_SSR_PHASE_BIAS:
			case +RtcmMessageType::GLO_SSR_PHASE_BIAS:
			case +RtcmMessageType::GAL_SSR_PHASE_BIAS:
			case +RtcmMessageType::QZS_SSR_PHASE_BIAS:
			case +RtcmMessageType::BDS_SSR_PHASE_BIAS:
			case +RtcmMessageType::SBS_SSR_PHASE_BIAS:
			{
				auto ssrPBMap = mongoReadPhaseBias(ssrMeta, masterIod, sys);
				
				auto buffer = encodeSsrPhase(ssrPBMap, messCode);
				bool write = encodeWriteMessageToBuffer(buffer);
				
				if (write == false)
				{
					std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
				}
				
				break;			
			}
			case +RtcmMessageType::GPS_SSR_CODE_BIAS:
			case +RtcmMessageType::GLO_SSR_CODE_BIAS:
			case +RtcmMessageType::GAL_SSR_CODE_BIAS:
			case +RtcmMessageType::QZS_SSR_CODE_BIAS:
			case +RtcmMessageType::BDS_SSR_CODE_BIAS:
			case +RtcmMessageType::SBS_SSR_CODE_BIAS:
			{
				auto ssrCBMap = mongoReadCodeBias(ssrMeta, masterIod, sys);
				
				auto buffer = encodeSsrCode(ssrCBMap, messCode);
				bool write = encodeWriteMessageToBuffer(buffer);
				
				if (write == false)
				{
					std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
				}
				
				break;
			}
			case +RtcmMessageType::GPS_SSR_COMB_CORR:
			case +RtcmMessageType::GLO_SSR_COMB_CORR:
			case +RtcmMessageType::GAL_SSR_COMB_CORR:
			case +RtcmMessageType::QZS_SSR_COMB_CORR:
			case +RtcmMessageType::BDS_SSR_COMB_CORR:
			case +RtcmMessageType::SBS_SSR_COMB_CORR:
			case +RtcmMessageType::GPS_SSR_ORB_CORR:
			case +RtcmMessageType::GLO_SSR_ORB_CORR:
			case +RtcmMessageType::GAL_SSR_ORB_CORR:
			case +RtcmMessageType::QZS_SSR_ORB_CORR:
			case +RtcmMessageType::BDS_SSR_ORB_CORR:
			case +RtcmMessageType::SBS_SSR_ORB_CORR:
			case +RtcmMessageType::GPS_SSR_CLK_CORR:
			case +RtcmMessageType::GLO_SSR_CLK_CORR:
			case +RtcmMessageType::GAL_SSR_CLK_CORR:
			case +RtcmMessageType::QZS_SSR_CLK_CORR:
			case +RtcmMessageType::BDS_SSR_CLK_CORR:
			case +RtcmMessageType::SBS_SSR_CLK_CORR:
			case +RtcmMessageType::GPS_SSR_HR_CLK_CORR:
			case +RtcmMessageType::GLO_SSR_HR_CLK_CORR:
			case +RtcmMessageType::GAL_SSR_HR_CLK_CORR:
			case +RtcmMessageType::QZS_SSR_HR_CLK_CORR:
			case +RtcmMessageType::BDS_SSR_HR_CLK_CORR:
			case +RtcmMessageType::SBS_SSR_HR_CLK_CORR:
			{
				ssrOutMap 	= mongoReadOrbClk(t0, ssrMeta, masterIod, sys);

				calculateSsrComb(t0, updateInterval, ssrMeta, masterIod, ssrOutMap);
				
				auto buffer = encodeSsrOrbClk(ssrOutMap, messCode);
				bool write = encodeWriteMessageToBuffer(buffer);
				
				if (acsConfig.trace_level > 5)
				{
					debugSSR(t0, targetTime, sys, ssrOutMap);
					
					if (write == false)
					{
						std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
					}
				}

				break;
			}
			case +RtcmMessageType::GPS_SSR_URA:
			case +RtcmMessageType::GLO_SSR_URA:
			case +RtcmMessageType::GAL_SSR_URA:
			case +RtcmMessageType::QZS_SSR_URA:
			case +RtcmMessageType::BDS_SSR_URA:
			case +RtcmMessageType::SBS_SSR_URA:
			{
				auto buffer = encodeSsrUra(ssrOutMap, messCode);
				bool write = encodeWriteMessageToBuffer(buffer);
				
				if (write == false)
				{
					std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
				}
				
				break;
			}
			case +RtcmMessageType::GPS_EPHEMERIS:
			case +RtcmMessageType::BDS_EPHEMERIS:
			case +RtcmMessageType::QZS_EPHEMERIS:
			case +RtcmMessageType::GAL_FNAV_EPHEMERIS:
			case +RtcmMessageType::GAL_INAV_EPHEMERIS:
			{
				bool write = false;
				
				for (auto& sat : getSysSats(sys))
				{
					auto eph = mongoReadEphemeris(targetTime, sat, messCode);
					
					if (eph.toe == GTime::noTime())
						continue;

					auto buffer = encodeEphemeris(eph, messCode);
					write |= encodeWriteMessageToBuffer(buffer);
				}

				if (write == false)
				{
					std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
				}

				break;
			}
			case +RtcmMessageType::GLO_EPHEMERIS:
			{
				bool write = false;
					
				for (auto& sat : getSysSats(sys))
				{
					auto geph = mongoReadGloEphemeris(targetTime, sat);

					if (geph.toe == GTime::noTime())
						continue;

					auto buffer = encodeEphemeris(geph, messCode);
					write |= encodeWriteMessageToBuffer(buffer);
				}

				if (write == false)
				{
					std::cout << "RtcmMessageType::" << messCode._to_string() << " was not written" << std::endl;
				}

				break;
			}

			case +RtcmMessageType::IGS_SSR:
			{
				SSRAtm							ssrAtm;
				map<E_Sys, map<SatSys, SSROut>> SSROutmaps;
				map<E_Sys, SsrCBMap>			SSRCodmaps;
				map<E_Sys, SsrPBMap>			SSRPhsmaps;
				map<E_Sys, SsrUraMap>			SSRURAmaps;
				
				IgsSSRSubtype					lastSubType = IgsSSRSubtype::IGS_SSR_GPS_NONE;
				map<IgsSSRSubtype ,bool>		approvedMessages;
				
				E_Sys sys;
				for (auto [subType, subUdi] : msgOpts.igs_udi)
				{
					if	(  subUdi == 0
						|| ((long int)targetTime) % subUdi != 0)
					{
						continue;
					}
					int subUdiIndex = getUdiIndex(subUdi);
					ssrMeta.updateIntIndex	= subUdiIndex;
					if (ssrMeta.updateIntIndex == 0)	t0 = targetTime;
					else								t0 = targetTime + subUdi/2.0;
					
					switch (IGS_SSR_group(subType, sys))
					{
						case 1:
						case 2:
						case 3:
						case 4:
						{
							auto sysOutMap = mongoReadOrbClk(t0, ssrMeta, masterIod, sys);
							calculateSsrComb(t0, subUdi, ssrMeta, masterIod, sysOutMap);
							if (sysOutMap.empty() == false)
							{
								lastSubType					= subType;
								SSROutmaps[sys]				= sysOutMap;
								approvedMessages[subType]	= true;
							}
							break;
						}
						case 5:
						{
							auto sysCBMap  = mongoReadCodeBias(ssrMeta, masterIod, sys);
							if (sysCBMap.empty() == false)
							{
								lastSubType					= subType;
								SSRCodmaps[sys]				= sysCBMap;
								approvedMessages[subType]	= true;
							}
							break;
						}
						case 6:
						{
							auto sysPBMap  = mongoReadPhaseBias(ssrMeta, masterIod, sys);
							if (sysPBMap.empty() == false)
							{
								lastSubType					= subType;
								SSRPhsmaps[sys]				= sysPBMap;
								approvedMessages[subType]	= true;
							}
							break;
						}
						case 7:
						{
							// auto sysUraMap = mongoReadUra(t0, ssrMeta, masterIod, sys);	// Eugene: use sysOutMap?
							// if (sysUraMap.empty() == false)
							// {
							// 	lastSubType					= subType;
							// 	SSRURAmaps[sys]				= sysUraMap;
							// 	approvedMessages[subType]	= true;
							// }
							break;
						}
						case 8:
						{
							ssrAtm = mongoReadIGSIonosphere(targetTime, ssrMeta, masterIod);
							if (ssrAtm.atmosGlobalMap.empty() == false)
							{
								lastSubType					= subType;
								approvedMessages[subType]	= true;
							}
							break;
						}
					}
				}

				if (approvedMessages.empty())
					break;
					
				for (auto [subType, subUdi] : msgOpts.igs_udi)
				{
					if (approvedMessages[subType] == false)
						continue;
					
					bool last = (lastSubType == subType);
					
					switch(IGS_SSR_group (subType, sys))
					{
						case 1: {auto buffer = encodeIGS_ORB(SSROutmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 2: {auto buffer = encodeIGS_CLK(SSROutmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 3: {auto buffer = encodeIGS_CMB(SSROutmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 4: {auto buffer = encodeIGS_HRC(SSROutmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 5: {auto buffer = encodeIGS_COD(SSRCodmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 6: {auto buffer = encodeIGS_PHS(SSRPhsmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 7: {auto buffer = encodeIGS_URA(SSRURAmaps[sys], sys, last);		encodeWriteMessageToBuffer(buffer);		break;}
						case 8: {auto buffer = encodeIGS_ATM(ssrAtm,               last);		encodeWriteMessageToBuffer(buffer);		break;}
					}
				}
				break;
			}
			case +RtcmMessageType::COMPACT_SSR:
			{
				break;	
			}
			default:
				BOOST_LOG_TRIVIAL(error) << "Error, attempting to upload incorrect message type: " << messCode << std::endl;
		}
	}
	
	std::stringstream messStream;
	encodeWriteMessages(messStream);

	messStream.seekg(0, messStream.end);
	int length = messStream.tellg();
	messStream.seekg(0, messStream.beg);
			
	BOOST_LOG_TRIVIAL(debug) << "Called " << __FUNCTION__ << " MessageLength : " << length << std::endl;
	if (length != 0)
	{
		vector<char> data;
		data.resize(length);
		
		outMessagesMtx.lock();
		std::ostream chunkedStream(&outMessages);
		chunkedStream << std::uppercase << std::hex << length << "\r\n";
		
		messStream		.read	(&data[0], length);
		chunkedStream	.write	(&data[0], length);
		chunkedStream << "\r\n";        
	
		if (url.protocol == "https")	{	boost::asio::async_write(*_sslsocket,	outMessages, boost::bind(&NtripUploader::write_handler, this, bp::error));}
		else							{	boost::asio::async_write(*_socket,		outMessages, boost::bind(&NtripUploader::write_handler, this, bp::error));}

		previousTargetTime = targetTime;
	}
}

void NtripUploader::startBroadcast()
{
// 	BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " Starting Send Loop.\n";

	sendTimer.expires_from_now(boost::posix_time::seconds(1));
	sendTimer.async_wait(boost::bind(&NtripUploader::messageTimeout_handler, this, bp::error));
}

void NtripUploader::connected()
{
	BOOST_LOG_TRIVIAL(info) << "Uploader connected.\n";
	
	// Although there should be no downloading attempting to download monitors the socket connection.
	start_read(true);
	
	startBroadcast();
}




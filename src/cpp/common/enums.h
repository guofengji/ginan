

#pragma once

#define BETTER_ENUMS_DEFAULT_CONSTRUCTOR(Enum) \
public:                                      \
	Enum() = default;

#define BETTER_ENUMS_MACRO_FILE "enum_macros.h"	//BETTER_ENUM Extended
#include "enum.h"	//BETTER_ENUM

typedef enum
{
	FTYPE_NONE,

	/* Base carrier frequencies */
	F1 = 1,		 	//  1575.42  MHz: GPS L1, GAL E1,  BDS B1,  QZS L1,  SBS L1,
	F2 = 2,			//  1227.60  MHz: GPS L2, QZS L2,
	F5 = 5,			//  1176.45  MHz: GPS L5, GAL E5A, BDS B2A, QZS L5, SBS L5
	F6 = 6,			//  1278.75  MHz: GAL E6, QZS L6,
	F7 = 7,			//  1207.14  MHz: GAL E5B, BDS B2B
	F8 = 8,			//  1191.795 MHz: GAL E5, BDS B2,
	G1 = 11, 		// ~1602     MHz: GLO G1,
	G2 = 12, 		// ~1246     MHz: GLO G2,
	G3 = 13,		//  1202.025 MHz: GLO G3,
	G4 = 14,		//  1600.995 MHz  GLO G1A
	G6 = 16,		//  1248.08  MHz: GLO G2A,
	B1 = 21,		//  1561.098 MHz: BDS B2-1, 
	B3 = 23,		//  1268.52  MHz: BDS B3
	I9 = 39,		//  2492.028 MHz: IRN S9
	NUM_FTYPES,

	/* Combined signals */
	FTYPE_IF12 = 102,
	FTYPE_IF15 = 105,
	FTYPE_IF25 = 205
} E_FType;

typedef enum
{
	CODE,
	PHAS,
	NUM_MEAS_TYPES
} E_MeasType;

typedef enum
{
	SVH_OK,
	SVH_UNHEALTHY = -1		//implicitly used in rtcm
} E_Svh;


BETTER_ENUM(E_Solution,		short int,
	NONE,
	SINGLE,
	SINGLE_X,
	PPP
)

BETTER_ENUM(E_Radio,		short int,
	TRANSMITTER,
	RECEIVER
)


BETTER_ENUM(E_Sys,			short int,
			NONE,
			GPS,
			GAL,
			GLO,
			QZS,
			SBS,
			BDS,
			LEO,
			SUPPORTED,
			IRN,
			IMS,
			COMB)

BETTER_ENUM(E_OffsetType,	short int,
			UNSPECIFIED,
			APC,
			COM)

BETTER_ENUM(E_TrigType,		short int,
			CONSTANT,
			COS,
			SIN)


BETTER_ENUM(KF,				short int,
	NONE,
	ONE,
	
	REC_POS,
	REC_POS_RATE,
	STRAIN_RATE,
	
	POS,
	VEL,
	ACC,
	
	HEADING,
	
	SAT_POS,
	SAT_POS_RATE,
	
	
	REF_SYS_BIAS,
	
	REC_CLOCK,			REC_SYS_BIAS			= REC_CLOCK,
	REC_CLOCK_RATE,		REC_SYS_BIAS_RATE		= REC_CLOCK_RATE,
	REC_CLOCK_RATE_GM,	REC_SYS_BIAS_RATE_GM	= REC_CLOCK_RATE_GM,
	
	SAT_CLOCK,
	SAT_CLOCK_RATE,
	SAT_CLOCK_RATE_GM,
	
	TROP,
	TROP_GRAD,
	
	ORBIT_PTS,
	SRP,
	
	
	IONOSPHERIC,
	IONO_STEC,
	
	DCB,
	
	EOP,
	EOP_RATE,
	EOP_ADJUST,
	EOP_RATE_ADJUST,
	
	CALC,

	SLR_REC_RANGE_BIAS,
	SLR_REC_TIME_BIAS,

	XFORM_XLATE,
	XFORM_RTATE,
	XFORM_SCALE,


	AMBIGUITY,
	CODE_BIAS,
	PHASE_BIAS,
	
	Z_AMB,
	WL_MEAS,
	WL_PIV,
	
	CODE_MEAS,
	PHAS_MEAS,
	
	REFERENCE,
	
	ORBIT,				FIRST_ORBIT_STATE	= ORBIT,
	EMP_DYB_0,		
	EMP_DYB_1C,
	EMP_DYB_1S,
	EMP_DYB_2C,
	EMP_DYB_2S,
	EMP_DYB_3C,
	EMP_DYB_3S,
	EMP_DYB_4C,
	EMP_DYB_4S,
	SRP_DYB_0,
	SRP_DYB_1C,
	SRP_DYB_1S,
	SRP_DYB_2C,
	SRP_DYB_2S,
	SRP_DYB_3C,
	SRP_DYB_3S,
	SRP_DYB_4C,
	SRP_DYB_4S,			LAST_ORBIT_STATE	= SRP_DYB_4S,
	
	RANGE
	
)


BETTER_ENUM(KEPLER,				short int,
			LX,
			LY,
			LZ,
			EU,
			EV,
			M
)

BETTER_ENUM(E_Relativity,	short int,
			OFF,
			ON)

BETTER_ENUM(E_ChiSqMode,	int,
			NONE,
			INNOVATION,
			MEASUREMENT,
			STATE)

BETTER_ENUM(E_TropModel,		int,
			VMF3,
			GPT2)

BETTER_ENUM(E_NoiseModel,		int,
			UNIFORM,
			ELEVATION_DEPENDENT)

BETTER_ENUM(E_LogLevel,			int,
			DEBUG,
			WARN,
			ERROR)

BETTER_ENUM(E_IonoModel,		int,
			NONE,
			MEAS_OUT,
			BSPLINE,
			SPHERICAL_CAPS,
			SPHERICAL_HARMONICS,
			LOCAL)

BETTER_ENUM(E_IonoMode,			int,
			OFF,						///< ionosphere option: correction off 
			BROADCAST,					///< ionosphere option: broadcast model 
			SBAS,						///< ionosphere option: SBAS model 
			IONO_FREE_LINEAR_COMBO,		///< ionosphere option: L1/L2 or L1/L5 iono-free LC 
			ESTIMATE,					///< ionosphere option: estimation 
			TOTAL_ELECTRON_CONTENT,		///< ionosphere option: IONEX TEC model 
			QZS,						///< ionosphere option: QZSS broadcast model 
			LEX,						///< ionosphere option: QZSS LEX ionospehre 
			STEC)						///< ionosphere option: SLANT TEC model 

BETTER_ENUM(E_IonoMapFn,		int,
			SLM,						///< single layer model mapping function
			MSLM,						///< modified single layer model mapping function
			MLM,						///< multiple layer model mapping function
			KLOBUCHAR)					///< Klobuchar mapping function

BETTER_ENUM(E_IonoFrame,		int,
			EARTH_FIXED,				///< Earth-fixed reference frame
			SUN_FIXED)					///< Sun-fixed reference frame

BETTER_ENUM(E_Period,			int,
			SECOND	= 1, 				
			MINUTE	= 60, 		
			HOUR	= 60 * 60,		
			DAY		= 60 * 60 * 24, 	
			WEEK	= 60 * 60 * 24 * 7,	
			YEAR	= 60 * 60 * 24 * 365,
			
			SECONDS			= SECOND,	MINUTES			= MINUTE,	HOURS		= HOUR,	DAYS		= DAY,	WEEKS		= WEEK,	YEARS		= YEAR,
			SEC				= SECOND,	MIN				= MINUTE,	HR			= HOUR,	DY			= DAY,	WK			= WEEK,	YR			= YEAR,
			SECS			= SECOND,	MINS			= MINUTE,	HRS			= HOUR,	DYS			= DAY,	WKS			= WEEK,	YRS			= YEAR,
			SQRT_SEC		= SECOND,	SQRT_MIN		= MINUTE,	SQRT_HR		= HOUR,	SQRT_DY		= DAY,	SQRT_WK		= WEEK,	SQRT_YR		= YEAR,
			SQRT_SECS		= SECOND,	SQRT_MINS		= MINUTE,	SQRT_HRS	= HOUR,	SQRT_DYS	= DAY,	SQRT_WKS	= WEEK,	SQRT_YRS	= YEAR,
			SQRT_SECOND		= SECOND,	SQRT_MINUTE		= MINUTE,	SQRT_HOUR	= HOUR,	SQRT_DAY	= DAY,	SQRT_WEEK	= WEEK,	SQRT_YEAR	= YEAR,
			SQRT_SECONDS	= SECOND,	SQRT_MINUTES	= MINUTE,	SQRT_HOURS	= HOUR,	SQRT_DAYS	= DAY,	SQRT_WEEKS	= WEEK,	SQRT_YEARS	= YEAR)

BETTER_ENUM(E_TimeSys, int,
			NONE,			///< NONE for unknown
			GPST,			///< GPS Time
			GLONASST,		///< GLONASS Time
			GST,			///< Galileo System Time
			BDT,			///< BeiDou Time
			QZSST,			///< QZSS Time
			TAI,			///< International Atomic Time
			UTC,			///< Universal Coordinated Time
			UT1,			///< Universal Time corrected for polar motion
			TT)				///< Terrestrial Time

BETTER_ENUM(E_PosFrame, int,
			NONE,
			XYZ,
			NED,
			RTN)

BETTER_ENUM(E_ObxFrame,	short int,
			OTHER,
			ECEF,
			ECI,
			BCRS)

BETTER_ENUM(E_FilterMode, int,
			LSQ,
			KALMAN)

BETTER_ENUM(E_Inverter, int,
			NONE,
			INV,
			LLT,
			LDLT,
			COLPIVHQR,
			BDCSVD,
			JACOBISVD,
			FULLPIVLU,		FIRST_UNSUPPORTED = FULLPIVLU,
			FULLPIVHQR)

BETTER_ENUM(E_ObsDesc, int,
	C,					// Code / Pseudorange
	L,					// Phase
	D,					// Doppler
	S,					// Raw signal strength (carrier to noise ratio)
	X 					// Receiver channel numbers
)

BETTER_ENUM(E_ObsCode, int,
	NONE  = 0 ,			///< none or unknown 
	L1C   = 1 ,			///< L1C/A,G1C/A,E1C		(GPS,GLO,GAL,QZS,SBS) 
	L1P   = 2 ,			///< L1P,G1P    			(GPS,GLO) 
	L1W   = 3 ,			///< L1 Z-track 			(GPS) 
	L1Y   = 4 ,			///< L1Y        			(GPS) 
	L1M   = 5 ,			///< L1M        			(GPS) 
	L1N   = 6 ,			///< L1codeless 			(GPS) 
	L1S   = 7 ,			///< L1C(D)     			(GPS,QZS) 
	L1L   = 8 ,			///< L1C(P)     			(GPS,QZS) 
	L1E   = 9 ,			///< L1C/B      			(QZS) 
	L1A   = 10,			///< E1A        			(GAL) 
	L1B   = 11,			///< E1B        			(GAL) 
	L1X   = 12,			///< E1B+C,L1C(D+P)			(GAL,QZS) 
	L1Z   = 13,			///< E1A+B+C,L1-SAIF		(GAL,QZS) 
	L2C   = 14,			///< L2C/A,G1C/A			(GPS,GLO) 
	L2D   = 15,			///< L2 L1C/A-(P2-P1)		(GPS) 
	L2S   = 16,			///< L2C(M)     			(GPS,QZS) 
	L2L   = 17,			///< L2C(L)     			(GPS,QZS) 
	L2X   = 18,			///< L2C(M+L),B1-2I+Q		(GPS,QZS,BDS) 
	L2P   = 19,			///< L2P,G2P    			(GPS,GLO) 
	L2W   = 20,			///< L2 Z-track 			(GPS) 
	L2Y   = 21,			///< L2Y        			(GPS) 
	L2M   = 22,			///< L2M        			(GPS) 
	L2N   = 23,			///< L2codeless 			(GPS) 
	L5I   = 24,			///< L5/E5aI    			(GPS,GAL,QZS,SBS) 
	L5Q   = 25,			///< L5/E5aQ    			(GPS,GAL,QZS,SBS) 
	L5X   = 26,			///< L5/E5aI+Q  			(GPS,GAL,QZS,SBS) 
	L7I   = 27,			///< E5bI,B2aI  			(GAL,BDS) 
	L7Q   = 28,			///< E5bQ,B2aQ  			(GAL,BDS) 
	L7X   = 29,			///< E5bI+Q,B2aI+Q			(GAL,BDS) 
	L6A   = 30,			///< E6A        			(GAL) 
	L6B   = 31,			///< E6B        			(GAL) 
	L6C   = 32,			///< E6C        			(GAL) 
	L6X   = 33,			///< E6B+C,LEXS+L,B3I+Q 	(GAL,QZS,BDS) 
	L6Z   = 34,			///< E6A+B+C    			(GAL) 
	L6S   = 35,			///< L6S    				(QZS) 
	L6L   = 36,			///< L6L    				(QZS)
	L8I   = 37,			///< E5(a+b)I   			(GAL)
	L8Q   = 38,			///< E5(a+b)Q   			(GAL)
	L8X   = 39,			///< E5(a+b)I+Q 			(GAL)
	L2I   = 40,			///< B1-2I      			(BDS)
	L2Q   = 41,			///< B1-2Q      			(BDS)
	L6I   = 42,			///< B3I        			(BDS)
	L6Q   = 43,			///< B3Q        			(BDS)
	L3I   = 44,			///< G3I        			(GLO)
	L3Q   = 45,			///< G3Q        			(GLO)
	L3X   = 46,			///< G3I+Q      			(GLO)
	L1I   = 47,			///< B1I        			(BDS)
	L1Q   = 48,			///< B1Q        			(BDS)
	L4A	  = 49,			///< L1OCd					(GLO)
	L4B	  = 50,			///< L1OCp					(GLO)
	L4X	  = 51,			///< L1OCd+L1OCp			(GLO)
	L6E	  = 52,			///< L6E					(QZS)
	L1D	  = 53,			///< B1D        			(BDS)
	L5D	  = 54,			///< B2aD       			(BDS)			
	L5P	  = 55,			///< B2aP       			(BDS)
	L9A	  = 57,			///< S9 A SPS				(IRN)
	L9B	  = 58,			///< S9 B RS(D)        		(IRN)
	L9C	  = 59,			///< S9 C RS(P)       		(IRN)			
	L9X	  = 60,			///< S9 B+C       			(IRN)
	L5A   = 61,			///< L5 A SPS				(IRN)
	L5B   = 62,			///< L5 B RS(D)				(IRN)
	L5C   = 63,			///< L5 C RS(P)				(IRN)
	L5Z   = 64,			///< L5 B+C       			(IRN)
	L6D	  = 65,			///< L6 Data       			(BDS)
	L6P	  = 66,			///< L6 Pilot       		(BDS)
	L7D	  = 67,			///< L7 Data       			(BDS)
	L7P	  = 68,			///< L7 Pilot       		(BDS)
	L7Z	  = 69,			///< L7 Data+Pilot       	(BDS)
	L8D	  = 70,			///< L8 Data       			(BDS)
	L8P	  = 71,			///< L8 Pilot       		(BDS)
	L8Z	  = 72			///< L8 Data+Pilot       	(BDS)
)

BETTER_ENUM(E_ObsCode2, int,
	NONE,
	P1,
	P2,
	C1,
	C2,
	C3,
	C4,
	C5,
	C6,
	C7,
	C8,
	L1,
	L2,
	L3,
	L4,
	L5,
	L6,
	L7,
	L8
)

BETTER_ENUM(E_ARmode,	short int,
			OFF,
			ROUND,
			ITER_RND,
			BOOTST,
			LAMBDA,
			LAMBDA_ALT,
			LAMBDA_AL2,
			LAMBDA_BIE) 

BETTER_ENUM(E_AmbTyp,	short int,
			NONE,
			NL12,
			WL12,
			WL23,
			UCL1,
			UCL2,
			UCL3)

BETTER_ENUM(E_NavRecType,	short int,
			NONE,			///< NONE for unknown */
			EPH,			///< Ephemerides data including orbit, clock, biases, accuracy and status parameters */
			STO,			///< System Time and UTC proxy offset parameters */
			EOP,			///< Earth Orientation Parameters */
			ION)			///< Global/Regional ionospheric model parameters */

BETTER_ENUM(E_NavMsgType,	short int,
			NONE,			///< NONE for unknown 
			LNAV,			///< GPS/QZSS/NavIC Legacy Navigation Messages 
			FDMA,			///< GLONASS Legacy FDMA Navigation Message 
			FNAV,			///< Galileo Free Navigation Message 
			INAV,			///< Galileo Integrity Navigation Message 
			IFNV,			///< Galileo INAV or FNAV Navigation Message 
			D1,				///< BeiDou-2/3 MEO/IGSO Navigation Message 
			D2,				///< BeiDou-2/3 GEO Navigation Message 
			D1D2,			///< BeiDou-2/3 MEO/IGSO and GEO Navigation Message 
			SBAS,			///< SBAS Navigation Message 
			CNAV,			///< GPS/QZSS CNAV Navigation Message 
			CNV1,			///< BeiDou-3 CNAV-1 Navigation Message 
			CNV2,			///< GPS/QZSS CNAV-2 Navigation Message	  BeiDou-3 CNAV-2 Navigation Message 
			CNV3,			///< BeiDou-3 CNAV-3 Navigation Message 
			CNVX)			///< GPS/QZSS CNAV or CNAV-2 Navigation Message  BeiDou-3 CNAV-1, CNAV-2 or CNAV-3 Navigation Message 

BETTER_ENUM(E_SatType,	short int,
			NONE,
			GEO,
			IGSO,
			MEO)

BETTER_ENUM(E_StoCode,	short int,
			NONE,
			GPUT,
			GLUT,
			GLGP,
			GAUT,
			GAGP,	GPGA = GAGP,	// From RINEX 3.04 the GPGA label is replaced by GAGP, while the value and sign for the Galileo minus GPS time offset remains unchanged.
			GAGL,
			BDUT,
			BDGP,
			BDGL,
			BDGA,
			QZUT,
			QZGP,
			QZGL,
			QZGA,
			QZBD,
			IRUT,
			IRGP,
			IRGL,
			IRGA,
			IRBD,
			IRQZ,
			SBUT,
			SBGP,
			SBGL,
			SBGA,
			SBBD,
			SBQZ,
			SBIR)

BETTER_ENUM(E_UtcId,	short int,
			NONE,
			UTC_USNO,
			UTC_SU,
			UTCGAL,
			UTC_NTSC,
			UTC_NICT,
			UTC_NPLI,
			UTCIRN,
			UTC_OP,
			UTC_NIST)

BETTER_ENUM(E_SbasId,	short int,
			NONE,
			WAAS,
			EGNOS,
			MSAS,
			GAGAN,
			SDCM,
			BDSBAS,
			KASS,
			A_SBAS,
			SPAN)

BETTER_ENUM(RtcmMessageType, uint16_t,
		NONE 				= 0,
		
		GPS_EPHEMERIS		= 1019,

		GLO_EPHEMERIS		= 1020,

		//GPS_NETWORK_RTK_RESIDUAL = 1030,
		//RECEIVER_AND_ANTENNA_DESC = 1033,

		BDS_EPHEMERIS 		= 1042,

		QZS_EPHEMERIS		= 1044,
		
		GAL_FNAV_EPHEMERIS	= 1045,
		GAL_INAV_EPHEMERIS	= 1046,
		
		GPS_SSR_ORB_CORR	= 1057,
		GPS_SSR_CLK_CORR	= 1058,
		GPS_SSR_CODE_BIAS	= 1059,
		GPS_SSR_COMB_CORR	= 1060,
		GPS_SSR_URA			= 1061,
		GPS_SSR_HR_CLK_CORR	= 1062,
		GPS_SSR_PHASE_BIAS	= 1265,

		GLO_SSR_ORB_CORR	= 1063,
		GLO_SSR_CLK_CORR	= 1064,
		GLO_SSR_CODE_BIAS	= 1065,
		GLO_SSR_COMB_CORR	= 1066,
		GLO_SSR_URA			= 1067,
		GLO_SSR_HR_CLK_CORR	= 1068,
		GLO_SSR_PHASE_BIAS	= 1266,

		MSM4_GPS 			= 1074,
		MSM5_GPS 			= 1075,
		MSM6_GPS 			= 1076,
		MSM7_GPS 			= 1077,
		
		MSM4_GLONASS 		= 1084,
		MSM5_GLONASS 		= 1085,
		MSM6_GLONASS 		= 1086,
		MSM7_GLONASS 		= 1087,
		
		MSM4_GALILEO 		= 1094,
		MSM5_GALILEO 		= 1095,
		MSM6_GALILEO 		= 1096,
		MSM7_GALILEO 		= 1097,
		
		MSM4_QZSS 			= 1114,
		MSM5_QZSS 			= 1115,
		MSM6_QZSS 			= 1116,
		MSM7_QZSS 			= 1117,
		
		MSM4_BEIDOU 		= 1124,
		MSM5_BEIDOU 		= 1125,
		MSM6_BEIDOU 		= 1126,
		MSM7_BEIDOU 		= 1127,
		
		//GLONASS_AUX_OPERATION_INFO = 1230
		
		GAL_SSR_ORB_CORR	= 1240,
		GAL_SSR_CLK_CORR	= 1241,
		GAL_SSR_CODE_BIAS	= 1242,
		GAL_SSR_COMB_CORR	= 1243,
		GAL_SSR_URA			= 1244,
		GAL_SSR_HR_CLK_CORR	= 1245,
		GAL_SSR_PHASE_BIAS	= 1267,
		
		QZS_SSR_ORB_CORR	= 1246,
		QZS_SSR_CLK_CORR	= 1247,
		QZS_SSR_CODE_BIAS	= 1248,
		QZS_SSR_COMB_CORR	= 1249,
		QZS_SSR_URA			= 1250,
		QZS_SSR_HR_CLK_CORR	= 1251,
		QZS_SSR_PHASE_BIAS	= 1268,
		
		SBS_SSR_ORB_CORR	= 1252,
		SBS_SSR_CLK_CORR	= 1253,
		SBS_SSR_CODE_BIAS	= 1254,
		SBS_SSR_COMB_CORR	= 1255,
		SBS_SSR_URA			= 1256,
		SBS_SSR_HR_CLK_CORR	= 1257,
		SBS_SSR_PHASE_BIAS	= 1269,
		
		BDS_SSR_ORB_CORR	= 1258,
		BDS_SSR_CLK_CORR	= 1259,
		BDS_SSR_CODE_BIAS	= 1260,
		BDS_SSR_COMB_CORR	= 1261,
		BDS_SSR_URA			= 1262,
		BDS_SSR_HR_CLK_CORR	= 1263,
		BDS_SSR_PHASE_BIAS	= 1270,
		
		COMPACT_SSR			= 4073,
		IGS_SSR				= 4076,
		CUSTOM				= 4082
)

BETTER_ENUM(CompactSSRSubtype, unsigned short,
		CMP_SSR_NONE	= 0,
		CMP_SSR_MSK		= 1, 
		CMP_SSR_ORB		= 2,
		CMP_SSR_CLK		= 3,
		CMP_SSR_COD		= 4,
		CMP_SSR_PHS		= 5,
		CMP_SSR_BIA		= 6,
		CMP_SSR_URA		= 7,
		CMP_SSR_TEC		= 8,
		CMP_SSR_GRD		= 9,
		CMP_SSR_SRV		= 10,
		CMP_SSR_CMB		= 11,
		CMP_SSR_ATM		= 12)

BETTER_ENUM(IgsSSRSubtype, unsigned short,
		IGS_SSR_GPS_NONE	= 0,
		IGS_SSR_GPS_ORB		= 21, 
		IGS_SSR_GPS_CLK		= 22,
		IGS_SSR_GPS_CMB		= 23,
		IGS_SSR_GPS_HRC		= 24,
		IGS_SSR_GPS_COD		= 25,
		IGS_SSR_GPS_PHS		= 26,
		IGS_SSR_GPS_URA		= 27,
		
		IGS_SSR_GLO_ORB		= 41, 
		IGS_SSR_GLO_CLK		= 42,
		IGS_SSR_GLO_CMB		= 43,
		IGS_SSR_GLO_HRC		= 44,
		IGS_SSR_GLO_COD		= 45,
		IGS_SSR_GLO_PHS		= 46,
		IGS_SSR_GLO_URA		= 47,
		
		IGS_SSR_GAL_ORB		= 61, 
		IGS_SSR_GAL_CLK		= 62,
		IGS_SSR_GAL_CMB		= 63,
		IGS_SSR_GAL_HRC		= 64,
		IGS_SSR_GAL_COD		= 65,
		IGS_SSR_GAL_PHS		= 66,
		IGS_SSR_GAL_URA		= 67,
		
		IGS_SSR_QZS_ORB		= 81, 
		IGS_SSR_QZS_CLK		= 82,
		IGS_SSR_QZS_CMB		= 83,
		IGS_SSR_QZS_HRC		= 84,
		IGS_SSR_QZS_COD		= 85,
		IGS_SSR_QZS_PHS		= 86,
		IGS_SSR_QZS_URA		= 87,
		
		IGS_SSR_BDS_ORB		= 101, 
		IGS_SSR_BDS_CLK		= 102,
		IGS_SSR_BDS_CMB		= 103,
		IGS_SSR_BDS_HRC		= 104,
		IGS_SSR_BDS_COD		= 105,
		IGS_SSR_BDS_PHS		= 106,
		IGS_SSR_BDS_URA		= 107,
		
		IGS_SSR_SBS_ORB		= 121, 
		IGS_SSR_SBS_CLK		= 122,
		IGS_SSR_SBS_CMB		= 123,
		IGS_SSR_SBS_HRC		= 124,
		IGS_SSR_SBS_COD		= 125,
		IGS_SSR_SBS_PHS		= 126,
		IGS_SSR_SBS_URA		= 127,
		
		IGS_SSR_IONVTEC		= 201
)

BETTER_ENUM(E_Source, short int,
		NONE,
		PRECISE,
		SSR,
		KALMAN,
		BROADCAST,
		NOMINAL,
		MODEL,
		REMOTE)

BETTER_ENUM(E_RTCMSubmessage,	short int,
		TIMESTAMP = 1
)

BETTER_ENUM(E_CrdEpochEvent, int,
	REC_RX			= 0,	// ground receive time (at SRP) (two-way)
	SAT_BN			= 1,	// spacecraft bounce time (two-way)
	REC_TX			= 2,	// ground transmit time (at SRP) (two-way)
	SAT_RX			= 3,	// spacecraft receive time (one-way)
	SAT_TX			= 4,	// spacecraft transmit time (one-way)
	REC_TX_SAT_RX	= 5,	// ground transmit time (at SRP) and spacecraft receive time (one-way)
	SAT_TX_REC_RX	= 6,	// spacecraft transmit time and ground receive time (at SRP) (one-way)
	NONE			= 7
)

BETTER_ENUM(E_ObsWaitCode,	short int,
		OK,
		EARLY_DATA,
		NO_DATA_WAIT,
		NO_DATA_EVER)

/* Options associated with solar radiation pressure models */
BETTER_ENUM(E_SRPModels,	int,
			CANNONBALL,
			BOXWING,
			ECOM,
			ECOM2)

BETTER_ENUM(E_GravMdl, 		short int,
			EGM08,
			GGM03S,
			GGM05S)


BETTER_ENUM(E_TidesMdl, 	short int,
			ELASTIC,
			ANELASTIC)			

BETTER_ENUM(E_ThirdBody,		short int,
			MERCURY		= 1,
			VENUS		= 2,
			EARTH		= 3,
			MARS		= 4,
			JUPITER		= 5,
			SATURN		= 6,
			URANUS		= 7,
			NEPTUNE		= 8,
			PLUTO		= 9,
			MOON		= 10,
			SUN			= 11)	//from jpl, do not modify


BETTER_ENUM(E_SigWarning, short int,
	SIG_OUTG	= 1,		// Minor (one signal) outage
	LOW_ELEV	= 2,		// Low elevation
	CYC_SLIP	= 3,		// Cycle slip
	MAJ_OUTG    = 4,		// Major (whole satellite/receiver) outage
	USR_DISC	= 5)		// User defined


BETTER_ENUM(E_SlrRangeType, short int, // from crd_v2.01.pdf p7
	TX_ONLY	= 0,		// no ranges (i.e., transmit time only)
	ONE_WAY	= 1,		// one-way ranging
	TWO_WAY	= 2,		// two-way ranging
	RX_ONLY	= 3,		// receive times only
	MIXED	= 3)		// mixed (for real-time data recording, and combination of one- and two-way ranging, e.g., T2L2)

BETTER_ENUM(E_UBXClass, short int,
	NAV			= 0x01,
	RXM			= 0x02,
	INF			= 0x04,
	ACK			= 0x05,
	CFG			= 0x06,
	MON			= 0x0A,
	AID			= 0x0B,
	TIM			= 0x0D)

BETTER_ENUM(E_RXMId, short int,
	SFRBX		= 0x13,
	MEASX		= 0x14,
	RAWX		= 0x15)

BETTER_ENUM(E_Month, short int,
	NONE,
	JAN,
	FEB,
	MAR,
	ARP,
	MAY,
	JUN,
	JUL,
	AUG,
	SEP,
	OCT,
	NOV,
	DEC)

BETTER_ENUM(E_FilePos, short int,
	COORD,
	CURR_TIME,
	TOTAL_TIME,
	PCDH,
	NUM_SAMPLES,
	FOOTER)

BETTER_ENUM(E_SSROutTiming, short int,
	GPS_TIME,
	LATEST_CLOCK_ESTIMATE)


BETTER_ENUM(E_Component, short int,
	NONE,
	X,
	LLH,
	P,
	DX,
	PREFIT,
	POSTFIT,
	VARIANCE,
	OBSERVED,
	HEADING,
	RANGE,
	SAT_CLOCK,
	REC_CLOCK,
	SAGNAC,
	RELATIVITY1,
	RELATIVITY2,
	REC_ANTENNA_DELTA,
	REC_PCO,
	SAT_PCO,
	REC_PCV,
	SAT_PCV,
	TIDES_SOLID,
	TIDES_POLE,
	TIDES_OTL,
	IONOSPHERIC_COMPONENT,
	IONOSPHERIC_COMPONENT1,
	IONOSPHERIC_COMPONENT2,
	IONOSPHERIC_MODEL,
	PHASE_WIND_UP,
	PHASE_AMBIGUITY,
	REC_RANGE_BIAS,
	REC_TIME_BIAS,
	REC_PHASE_BIAS,
	SAT_PHASE_BIAS,
	REC_CODE_BIAS,
	SAT_CODE_BIAS,
	TROPOSPHERE,
	EOP,
	NET_RESIDUAL,
	ORBIT_PT,
	SAT_REFLECTOR_DELTA,
	CENTRAL_FORCE,
	ALBEDO,
	INDIRECT_J2,
	EMPIRICAL,
	GENERAL_RELATIVITY,
	EGM,
	SRP,
	ANTENNA_THRUST,
	PLANETARY_PERTURBATION)



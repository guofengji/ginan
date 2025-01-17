

#include <iostream>
#include <fstream>
#include <vector>

using std::vector;

#include<boost/algorithm/string/split.hpp>
#include<boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

#include "staticField.hpp"
#include "gTime.hpp"

//todo aaron global
StaticField egm;

StaticField::StaticField(
	string 		filename,
	int 		degMax)
	:	filename	(filename),
		degMax		(degMax)
{
	readegm(filename);
	summary();
}


/** Read the static gravity field
* @todo include the time variable component of the gravity field.
*/
void StaticField::readegm(
	string filename)
{
	if (filename.empty())
	{
		return;
	}

	std::ifstream infile(filename);
	if (!infile)
	{
		return;
	}
	
	this->filename = filename;
	
	bool header = true;
	gfctC	= MatrixXd::Zero(degMax + 1, degMax + 1);
	gfctS	= MatrixXd::Zero(degMax + 1, degMax + 1);

	string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		if (header)
		{
			if (line.compare(0, 11, "end_of_head") == 0)
			{
				header = false;
			}
			else
			{
				if (line.length() != 0)
				{
					vector<string> split;
					boost::algorithm::split(split, line, boost::algorithm::is_any_of(" "), boost::token_compress_on);
					if		(split[0].compare("modelname")				== 0)			modelName				= 		split[1];			
					else if	(split[0].compare("earth_gravity_constant")	== 0)			earthGravityConstant	= stod(split[1]);			
					else if	(split[0].compare("radius")					== 0)			earthRadius				= stod(split[1]);		
					else if	(split[0].compare("max_degree")				== 0)			maxDegree				= stoi(split[1]);				
					else if	(split[0].compare("tide_system")			== 0)				
						{					
							if	(split[1].compare("tide_free")			== 0)			isTideFree = true;				
							else														isTideFree = false;
						}
				}
			}
		}
		else
		{
			int n;
			int m;
			double C;
			double S;
			double sigC;
			double sigS;
			string key;
			iss >> key >> n >> m >> C >> S >> sigC >> sigS ;
			if (n <= degMax)
			{
				if (key.compare(0, 3, "gfc") == 0)
				{
									gfctC(n, m) = C;
					if (m > 0)		gfctS(n, m) = S;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	initialised = true;
}

void StaticField::toZeroTide(
	bool convertToZero)
{
	double correction = -4.1736e-9;
	if	(  isTideFree
		&& convertToZero)
	{
		gfctC(2, 0) += correction;
	}
	if	( isTideFree	== false 
		&&convertToZero == false)
	{
		gfctC(2, 0) -= correction;
	}
}

void StaticField::summary()
{
	BOOST_LOG_TRIVIAL(info) << "read file : " << filename;
	BOOST_LOG_TRIVIAL(info) << "    - modelName         " << modelName;
	BOOST_LOG_TRIVIAL(info) << "    - max degree model  " << maxDegree;
	if (maxDegree >= degMax )
		BOOST_LOG_TRIVIAL(info) << "        => read only the first " << degMax << " degrees";
	else
		BOOST_LOG_TRIVIAL(warning) << "        => Maximum degree requested is higher than the model " << degMax << ">" << maxDegree ;
	BOOST_LOG_TRIVIAL(info) << "    - is tide_free      " << isTideFree ;
}

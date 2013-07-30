// $Id: speedtest.h 223 2008-04-07 16:56:58Z tb $
// Not really an include file, more a base file for the speed tests.

#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <limits>

// *** Speedtest Parameters ***

// speed test different buffer sizes in this range
const unsigned int buffermin = 16;
const unsigned int buffermax = 16 * 65536;
const unsigned int baserepeatsize = 65536;
const unsigned int minrepeats = 4;
const unsigned int measureruns = 16;

#ifndef _MSC_VER
#include <sys/time.h>
#else
#include <windows.h>
#endif

/// Time is measured using gettimeofday() or GetTickCount()
inline double timestamp()
{
#ifndef _MSC_VER
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 0.000001;
#else
    return timeGetTime() / 1000.0;
#endif
}

// *** Global Buffers and Settings for the Speedtest Functions ***

unsigned char	enckey[32];	/// 256 bit encryption key
unsigned char	enciv[16];	/// 16 byte initialization vector if needed.

unsigned char	buffer[buffermax];	/// encryption buffer
unsigned int	bufferlen;		/// currently tested buffer length

// *** run_test() ***

/**
 * This function will run a test routine multiple times with different buffer
 * sizes configured. It measures the time required to encrypt a number of
 * bytes. The average time and standard deviation are calculated and written to
 * a log file for gnuplot.
 */

template <void (*testfunc)()>
void run_test(const char* logfile)
{
    std::cout << "Speed testing for " << logfile << "\n";

    // Save the time required for each run.
    std::map<unsigned int, std::vector<double> > timelog;
	
    for(unsigned int fullrun = 0; fullrun < measureruns; ++fullrun)
    {
        unsigned int repeatsize = baserepeatsize;

	for(unsigned int bufflen = buffermin; bufflen <= buffermax; bufflen *= 2)
	{
REDO:
	    // because small time measurements are inaccurate, repeat very fast
	    // tests until the same amount of data is encrypted as in the large
	    // tests.
	    unsigned int repeat = repeatsize / bufflen;
	    if (repeat < minrepeats) repeat = minrepeats;

	    // std::cout << "Test: bufflen " << bufflen << " repeat " << repeat << "\n";

	    bufferlen = bufflen;

	    // fill buffer
	    for(unsigned int i = 0; i < bufferlen; ++i)
		buffer[i] = (uint8_t)i;

	    double ts1 = timestamp();

	    for(unsigned int testrun = 0; testrun < repeat; ++testrun)
	    {
		testfunc();
	    }

	    double ts2 = timestamp();

	    double tsdelta = ts2 - ts1;

	    // Adapt number of repetitions to the clock's accuracy.
	    if (tsdelta < 0.5) {
       		printf("Run %d bufferlen %d repeat %d took only %.2f sec. Increasing repetitons.\n",
		       fullrun, bufferlen, repeat, tsdelta);
		repeatsize *= 2;
		goto REDO;
	    }
	    else {
       		printf("Run %d bufferlen %d repeat %d took %.2f sec.\n",
		       fullrun, bufferlen, repeat, tsdelta);
	    }

	    // check buffer status after repeated en/decryption
	    for(unsigned int i = 0; i < bufferlen; ++i)
		assert(buffer[i] == (uint8_t)i);

	    timelog[bufferlen].push_back( tsdelta / (double)repeat );
	}
    }

    // Calculate and output statistics.
    std::ofstream of (logfile);

    // First output time absolute measurements
    for(std::map<unsigned int, std::vector<double> >::const_iterator ti = timelog.begin();
	ti != timelog.end(); ++ti)
    {
	const std::vector<double>& timelist = ti->second;

	double average = std::accumulate(timelist.begin(), timelist.end(), 0.0) / timelist.size();

	double variance = 0.0;
	for(unsigned int i = 0; i < timelist.size(); ++i)
	{
	    variance += (timelist[i] - average) * (timelist[i] - average);
	}
	variance = variance / (timelist.size() - 1);

	double stddev = std::sqrt(variance);

	if (timelist.size() == 1) { // only one run -> no variance or stddev
	    variance = stddev = 0.0;
	}

	double vmin = *std::min_element(timelist.begin(), timelist.end());
	double vmax = *std::max_element(timelist.begin(), timelist.end());

	of << std::setprecision(16);
	of << ti->first << " " << average << " " << stddev << " " << vmin << " " << vmax << "\n";
    }
    of << "\n\n";

    // Second output speed measurements
    for(std::map<unsigned int, std::vector<double> >::const_iterator ti = timelog.begin();
	ti != timelog.end(); ++ti)
    {
	const std::vector<double>& timelist = ti->second;

	double average = 0.0;
	double vmin = std::numeric_limits<double>::infinity();
	double vmax = 0;

	for(unsigned int i = 0; i < timelist.size(); ++i)
	{
	    average += (double)ti->first / timelist[i];
	    vmin = std::min<double>(vmin, (double)ti->first / timelist[i]);
	    vmax = std::max<double>(vmax, (double)ti->first / timelist[i]);
	}
	average /= timelist.size();

	double variance = 0.0;
	for(unsigned int i = 0; i < timelist.size(); ++i)
	{
	    double delta = ((double)ti->first / timelist[i]) - average;
	    variance += delta * delta;
	}
	variance = variance / (timelist.size() - 1);

	double stddev = std::sqrt(variance);

	if (timelist.size() == 1) { // only one run -> no variance or stddev
	    variance = stddev = 0.0;
	}

	of << std::setprecision(16);
	of << ti->first << " " << average << " " << stddev << " " << vmin << " " << vmax << "\n";
    }
    of.close();
}

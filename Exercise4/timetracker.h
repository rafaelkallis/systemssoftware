/*
 * Timetracker.h
 *
 *  Created on: Oct 28, 2015
 *      Author: rafael
 */

#ifndef TIMETRACKER_H_
#define TIMETRACKER_H_

#include <sys/time.h>
#include <stdio.h>

class Timetracker {
public:
	Timetracker();
	~Timetracker();

	void start  ();
	long stop   ();
	long check  ();
	void reset  ();

protected:
	timeval tracker_time;
	timeval diff;
	bool isRunning;
};

#endif /* TIMETRACKER_H_ */

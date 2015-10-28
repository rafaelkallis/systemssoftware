/*
 * Timetracker.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: rafael
 */

#include "timetracker.h"

Timetracker::Timetracker() {
	isRunning = false;
	tracker_time.tv_sec = tracker_time.tv_usec = diff.tv_sec = diff.tv_usec = 0;
}

Timetracker::~Timetracker() {

}

void Timetracker::start(){
	if (!isRunning) {
	        gettimeofday(&tracker_time, NULL);
	        isRunning = true;
	    }
}

long Timetracker::stop(){
    if (isRunning) {
        timeval current_time;
        gettimeofday(&current_time, NULL);

        diff.tv_sec  += current_time.tv_sec  - tracker_time.tv_sec;
        diff.tv_usec += current_time.tv_usec - tracker_time.tv_usec;

        isRunning = false;
    }

    return (1000*(diff.tv_sec) + (diff.tv_usec)/1000);
}

long Timetracker::check(){
    if (isRunning) {
        timeval current_time;
        gettimeofday(&current_time, NULL);

        diff.tv_sec  += current_time.tv_sec  - tracker_time.tv_sec;
        diff.tv_usec += current_time.tv_usec - tracker_time.tv_usec;
    }

    return (1000*(diff.tv_sec) + (diff.tv_usec)/1000);
}

void Timetracker::reset(){
	isRunning = false;
	tracker_time.tv_sec = tracker_time.tv_usec = diff.tv_sec = diff.tv_usec = 0;
}

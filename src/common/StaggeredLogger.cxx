//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2019 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "StaggeredLogger.hxx"

#include <ctime>

using namespace std::chrono;

namespace {
  string currentTimestamp()
  {
    std::tm now = BSPF::localTime();

    char formattedTime[100];
    formattedTime[99] = 0;
    std::strftime(formattedTime, 99, "%H:%M:%S", &now);

    return formattedTime;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StaggeredLogger::StaggeredLogger(const string& message, Logger logger)
  : myMessage(message),
    myCurrentEventCount(0),
    myIsCurrentlyCollecting(false),
    myCurrentIntervalSize(100),
    myMaxIntervalFactor(9),
    myCurrentIntervalFactor(1),
    myCooldownTime(1000),
    myTimer(new TimerManager()),
    myTimerCallbackId(0)
{
  if (logger) myLogger = logger;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StaggeredLogger::~StaggeredLogger()
{
  myTimer->clear(myTimerId);

  // make sure that the worker thread joins before continuing with the destruction
  delete myTimer;

  // the worker thread has joined and there will be no more reentrant calls ->
  // continue with destruction
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::setLogger(Logger logger)
{
  std::lock_guard<std::mutex> lock(myMutex);

  _setLogger(logger);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::_setLogger(Logger logger)
{
  myLogger = logger;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::log()
{
  std::lock_guard<std::mutex> lock(myMutex);

  _log();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::_log()
{
  if (!myIsCurrentlyCollecting) startInterval();

  ++myCurrentEventCount;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::logLine()
{
  if (!myLogger) return;

  high_resolution_clock::time_point now = high_resolution_clock::now();
  Int64 millisecondsSinceIntervalStart =
    duration_cast<duration<Int64, std::milli>>(now - myLastIntervalStartTimestamp).count();

  stringstream ss;
  ss
    << currentTimestamp() << ": "
    << myMessage
    << " (" << myCurrentEventCount << " times in "
      << millisecondsSinceIntervalStart << "  milliseconds"
    << ")";

  myLogger(ss.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::increaseInterval()
{
  if (myCurrentIntervalFactor >= myMaxIntervalFactor) return;

  ++myCurrentIntervalFactor;
  myCurrentIntervalSize *= 2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::decreaseInterval()
{
  if (myCurrentIntervalFactor <= 1) return;

  --myCurrentIntervalFactor;
  myCurrentIntervalSize /= 2;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::startInterval()
{
  if (myIsCurrentlyCollecting) return;

  myIsCurrentlyCollecting = true;

  high_resolution_clock::time_point now = high_resolution_clock::now();
  Int64 msecSinceLastIntervalEnd =
    duration_cast<duration<Int64, std::milli>>(now - myLastIntervalEndTimestamp).count();

  while (msecSinceLastIntervalEnd > myCooldownTime && myCurrentIntervalFactor > 1) {
    msecSinceLastIntervalEnd -= myCooldownTime;
    decreaseInterval();
  }

  myCurrentEventCount = 0;
  myLastIntervalStartTimestamp = now;

  myTimer->clear(myTimerId);
  myTimerId = myTimer->setTimeout(std::bind(&StaggeredLogger::onTimerExpired, this, ++myTimerCallbackId), myCurrentIntervalSize);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StaggeredLogger::onTimerExpired(uInt32 timerCallbackId)
{
  std::lock_guard<std::mutex> lock(myMutex);

  if (timerCallbackId != myTimerCallbackId) return;

  logLine();

  myIsCurrentlyCollecting = false;
  increaseInterval();

  myLastIntervalEndTimestamp = high_resolution_clock::now();
}

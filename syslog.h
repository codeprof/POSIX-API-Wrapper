/*
 * Copyright (C) 2011  Christian Eppler
 * File:        	syslog.h
 * Title:       	Header of syslog module
 * Author:      	Christian Eppler
 * E-Mail:      	weblooker@silentchris.de
 * Created on:  	18.10.2011
 * Version:     	$Rev: 268 $
 * Last modified: 	$Date: 2011-11-28 13:30:09 +0100 (Mon, 28 Nov 2011) $
 * Last modified by:$Author: silentchris $
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * IMPORTANT!!!
 * This is not the system syslog header.
 */

#ifndef SYSLOG_H_
#define SYSLOG_H_

/*includes*/
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>

#ifndef CUSTOMTYPES_H_
	#include "customtypes.h"
#endif

/**
 * Our Loglevels
 *
 * 2 and 1 are not for us I guess, 0 is normally emergency - we use it for
 * turning logging of
 */
typedef enum logLevel
{
	ldebug 	= 	7,
	linfo 	= 	6,
	lnotice = 	5,
	lwarn 	= 	4,
	lerror	= 	3
} LOGLEVEL;


// Error declarations for HRESULT
#define OK					1
#define ERROR 			   -1
#define LOG_LEVEL_TO_SMALL 	0

// define for message size
#define MLENGTH  1000		//Length of messages

/*Prototype*/

// Create a syslog entry
void syslogWrite(LOGLEVEL loglevel, char *logMessage);
// Create a syslog entry and stdout message
void syslogWriteDebug(LOGLEVEL loglevel, char *logMessage);

// Create a format syslog entry
int syslogWriteFormat(LOGLEVEL loglevel, const char* logMessage,...);
// Create a format syslog entry and format stdout message
int syslogWriteFormatDebug(LOGLEVEL loglevel, const char* logMessage,...);

// Setter functions (must be set before use of syslog module!)
BOOL syslogSetLoglevel(LOGLEVEL loglevel);
BOOL syslogSetDebug(BOOL debug);

// Getter functions for read Debug Status and loglevel
LOGLEVEL syslogGetLoglevel();
int syslogGetDebug();

#endif /* SYSLOG_H_ */

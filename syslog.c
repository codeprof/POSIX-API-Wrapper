/*
 * Copyright (C) 2011  Christian Eppler
 * File:        	syslog.c
 * Title:       	Modul for create syslog messages
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
 * This is not the system syslog file.
 */

/*includes*/
#include "syslog.h"

// Only for intern use
int syslogSetGetLogLevel(int switching,LOGLEVEL level,int debug);
int strcpyFromTo(char * msg,char * old,int from,int to);

/**
 * syslogWrite() \n
 * Function to write to syslog \n
 * @parm LOGLEVEL loglevel - level (info, notice, warn, error, ...) \n
 * @parm char *logMessage - the message that should be logged \n
 * @parm PTR_CHECKIT parameter - pointer for PTR_CHECKIT
 * @return void \n
 *
 * Use:
 * syslogWrite(lerror,"My message for /var/log/syslog");
 */
void syslogWrite(LOGLEVEL loglevel, char *logMessage)
{

	// Control loglevel
	if(loglevel > syslogGetLoglevel())
		return;

	// Set identification values for syslog
	// Make syslog entryie at /var/log/syslog
	openlog("linuxquiz", LOG_PID, LOG_USER);
	syslog(loglevel, logMessage);
	closelog();

	return;
}


/**
 * syslogWriteDebug() \n
 * Function to write to syslog and print debug message \n
 * @parm LOGLEVEL loglevel - level (info, notice, warn, error, ...) \n
 * @parm char *logMessage - the message that should be logged \n
 * @parm PTR_CHECKIT parameter - pointer for PTR_CHECKIT
 * @return void \n
 *
 * Use:
 * syslogWriteDebug(lerror,"My message for /var/log/syslog and stdout");
 */
void syslogWriteDebug(LOGLEVEL loglevel, char *logMessage)
{

	// Control loglevel
	if(loglevel > syslogGetLoglevel())
	{
		// Set identification values for syslog
		// Make syslog entryie at /var/log/syslog
		openlog("linuxquiz", LOG_PID, LOG_USER);
		syslog(loglevel, logMessage);
		closelog();
	}

	//print Debug message on console
	if(syslogGetDebug() == 1)
	{
		if(loglevel == lerror)
			printf("\033[49;0;31m%s\n\033[0m",logMessage);
		else
			printf("%s\n",logMessage);
	}
	return;
}

/**
 * syslogWriteFormatDebug()\n
 * Function to build format syslog message with debug output \n
 * \% is don't work \n
 * @param LOGLEVEL loglevel with the loglevel of the message \n
 * @param const char * logMessage with the format string (%i %f %s) \n
 * @return HRESULT with error code \n
 *
 * Use:
 * syslogWriteFormatDebug(lerror,"My formated message with number %i for %s and stdout",2,"/var/log/syslog");
 */
HRESULT syslogWriteFormatDebug(LOGLEVEL loglevel, const char * logMessage,...)
{
	// Variables
	va_list params;

	int x;							// Main loop index
	int y = 0;						// Position of read
	int numbers	 = 0;				// Numbers of variables
	int start =0;					// Save the start of part string
	char tmpMsg[MLENGTH];
	char newTmpMsg[MLENGTH] = "";
	char newMsg[MLENGTH] = "";
	char endMsg[MLENGTH] = "";			// The Message which will be used
	int lengthMsg;

	// Length of message for inner loop
	lengthMsg = strlen(logMessage);

	// Read how much variables
	while(y < lengthMsg)
	{
		if(logMessage[y] == '%')
			numbers++;

		y++;
	}
	// Position of read at next loop
	y = 0;

	// Initials access to parameter list
	va_start(params, numbers);

	// Copy message
	strncpy(tmpMsg,logMessage,MLENGTH);

	// Read all parameters
	for(x = 0; x < numbers;x++)
	{
		// Save start point
		start = y;

		// Go to %s %i %f %c
		while(1)
		{
			if(tmpMsg[y] == '%')
			{
				y++;
				break;
			}
			y++;

			// No % found / to match parameters
			if(y >= strlen(logMessage))
					return ERROR;
		}

		switch(tmpMsg[y])
		{
			case 's':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,char*));
				break;
			case 'f':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,double));
				break;
			case 'i':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,int));
				break;
			case 'd':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,int));
				break;
			default:
				return ERROR;
		}
		y++;
		//strcat(endMsg,newMsg);
		strcpyFromTo(endMsg,newMsg,0,strlen(newMsg));

		// Clean up puffers
		memset(newTmpMsg,'\0',MLENGTH);
		memset(newMsg,'\0',MLENGTH);
	}

	// Copy the end of the string to log message
	strcpyFromTo(endMsg,(char *)logMessage,y,strlen(logMessage));

	// Make syslog
	syslogWriteDebug(loglevel,endMsg);

	// Control loglevel
	if(loglevel > syslogGetLoglevel())
		return  LOG_LEVEL_TO_SMALL;

	// Close params read
	va_end(params);

	return OK;
}

/**
 * syslogWriteFormat() \n
 * Function to build syslog message without debug output \n
 * \% is don't work \n
 * @param LOGLEVEL loglevel with the loglevel of the message \n
 * @param const char * logMessage with the format string (%i %f %s) \n
 * @return HRESULT with error code \n
 *
 * Use:
 * syslogWriteFormatDebug(lerror,"My formated message with number %i for %s",2,"/var/log/syslog");
 */
HRESULT syslogWriteFormat(LOGLEVEL loglevel, const char * logMessage,...)
{
	// Control loglevel
	if(loglevel > syslogGetLoglevel())
		return  LOG_LEVEL_TO_SMALL;

	// Variables
	va_list params;
	int x;							// Main loop index
	int y = 0;						// Position of read
	int numbers	 = 0;				// Numbers of variables
	int start =0;					// Save the start of part string
	char tmpMsg[MLENGTH];
	char newTmpMsg[MLENGTH] = "";
	char newMsg[MLENGTH] = "";
	char endMsg[MLENGTH] = "";		// The Message which will be used
	int lengthMsg;

	// Length of message for inner loop
	lengthMsg = strlen(logMessage);

	// Read how much variables
	while(y < lengthMsg)
	{
		if(logMessage[y] == '%')
			numbers++;

		y++;
	}
	// Position of read at next loop
	y = 0;

	// Initials access to parameter list
	va_start(params, numbers);

	//Copy message
	strncpy(tmpMsg,logMessage,MLENGTH);

	// Read all parameters
	for(x = 0; x < numbers;x++)
	{
		// Save start point
		start = y;

		// Go to %s %i %f %c
		while(1)
		{
			if(tmpMsg[y] == '%')
			{
				y++;
				break;
			}
			y++;

			// No % found / to match parameters
			if(y >= strlen(logMessage))
					return ERROR;
		}

		switch(tmpMsg[y])
		{
			case 's':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,char*));
				break;
			case 'f':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,double));
				break;
			case 'i':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,int));
				break;
			case 'd':
				strcpyFromTo(newTmpMsg,tmpMsg,start,y+1);
				sprintf(newMsg,(const char *) newTmpMsg,va_arg(params,int));
				break;
			default:
				return ERROR;
		}
		y++;
		//strcat(endMsg,newMsg);
		strcpyFromTo(endMsg,newMsg,0,strlen(newMsg));

		// Clean up puffers;
		memset(newTmpMsg,'\0',MLENGTH);
		memset(newMsg,'\0',MLENGTH);
	}

	// Copy the end of the string to log message
	strcpyFromTo(endMsg,(char *)logMessage,y,strlen(logMessage));

	// Make syslog
	syslogWrite(loglevel,endMsg);

	// Close params read
	va_end(params);

	return OK;
}


/**
 * strcpyFromTo() \n
 * Copy string from position to position (Only for intern use) \n
 * @param char * msg the destination string \n
 * @param char * old the source string \n
 * @param int from with the start of the position \n
 * @param int to  with the end of position \n
 * @return int -1 for error and 1 for all right \n
 */
int strcpyFromTo(char * msg,char * old,int from,int to)
{
	int i = from;
	int lenght = strlen(msg);

	if(to < from)
		return -1;
	if(strlen(old) < to || strlen(old) < from)
		return -1;
	// Message to Long
	if((MLENGTH -1)-1 < to)
		return -1;

	while(i < to)
	{
		msg[lenght] = old[i];
		lenght = strlen(msg);
		i++;
	}
	return 1;
}


/**
 * syslogSetGetLogLevel() \n
 * Setter and getter for loglevel and debug modus (use wrapper functions is recommended !) \n
 * @param int switching 1 for set loglevel 2 for set debug \n
 * 		  and 3 for get loglevel 4 for get debug status \n
 * @param LOGLEVEL level \n
 * 		  ldebug = 7 \n
 *	      linfo = 6 \n
 *	      lnotice = 5 \n
 *	      lwarn = 4 \n
 *	      lerror = 3 \n
 * @param int debug 1 for start dbug and 0 for no debuging \n
 * @return 	int loglevel is switch on 2 and \n
 * 			if switch on 1 it return 1 on success and -1 for error \n
 */
int syslogSetGetLogLevel(int switching,LOGLEVEL level,int debug)
{
	// Variables
	static int stDebug;
	static int stLoglevel;

	// Setting values
	if(switching == 1)
		stDebug = debug;
	if(switching == 2)
		stLoglevel = level;

	// Getting values
	if(switching == 3)
		return (LOGLEVEL) stLoglevel;
	if(switching == 4)
		return stDebug;

	// Error
	return -1;
}

/**
 * syslogSetLoglevel() \n
 * Set the loglevel (must be set before use!)\n
 * @param LOGLEVEL loglevel \n
 * @return BOOL true or false \n
 */
BOOL syslogSetLoglevel(LOGLEVEL loglevel)
{
	if(loglevel == 0 || loglevel == 3 || loglevel == 4 || loglevel == 5 || loglevel == 6 || loglevel == 7)
	{
		syslogSetGetLogLevel(2,loglevel,0);
		return true;
	}
	return false;
}

/**
 * syslogSetDebug() \n
 * Set the debug modus (must be set before use!) \n
 * @param BOOL debug \n
 * @return BOOL true or false error \n
 */
BOOL syslogSetDebug(BOOL debug)
{
	if(debug == 1 || debug == 0)
	{
		syslogSetGetLogLevel(1,0,debug);
		return true;
	}
	return false;
}

/**
 * syslogGetLoglevel() \n
 * Geter for loglevel \n
 * @return LOGLEVEL loglevel \n
 */
LOGLEVEL syslogGetLoglevel()
{
	return syslogSetGetLogLevel(3,0,0);
}
/**
 * syslogGetDebug() \n
 * Geter for debug modus \n
 * @return int debug modus \n
 */
int syslogGetDebug()
{
	return syslogSetGetLogLevel(4,0,0);
}

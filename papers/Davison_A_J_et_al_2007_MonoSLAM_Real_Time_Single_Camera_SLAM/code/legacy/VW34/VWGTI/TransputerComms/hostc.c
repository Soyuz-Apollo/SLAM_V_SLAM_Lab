
/*
 --   ---------------------------------------------------------------------------
 --
 --      ISERVER  -  INMOS standard file server
 --
 --      hostc.c
 --
 --      Primary environment operations
 --
 --      Copyright (c) INMOS Ltd., 1988.
 --      All Rights Reserved.
 --	 Modifications for linux by Christoph Niemann
 --
 --   ---------------------------------------------------------------------------
*/



#include <stdio.h>
#include <string.h>
#include <time.h>

#include <linux/types.h>
#include <linux/termios.h>

#include "inmos.h"
#include "iserver.h"
#include "pack.h"


EXTERN BOOL CocoPops;		/*  for DEBUG  */
EXTERN BOOL VerboseSwitch;

EXTERN BYTE Tbuf[TRANSACTION_BUFFER_SIZE];

PRIVATE BYTE DataBuffer[MAX_SLICE_LENGTH+1];
PRIVATE int Size;

#define ORG_MODE 0
#define GET_MODE 1
#define POLL_MODE 2

PRIVATE BOOL TermMode = ORG_MODE;

PRIVATE struct termios OrgMode, CurMode;


PUBLIC VOID HostEnd()
{
   ioctl(0,TCSETS,&OrgMode);
}


PUBLIC VOID ResetTerminal()
{
   if ( TermMode != ORG_MODE )
      {
	 ioctl(0, TCSETS, &OrgMode);
	 TermMode = ORG_MODE;
      }
}


PUBLIC VOID HostBegin()
{
   ioctl(0,TCGETS,&OrgMode);
   ioctl(0,TCGETS,&CurMode);
}

/*
 *   GetAKey
 */

PUBLIC BYTE GetAKey()
{
   BYTE c;

   if ( TermMode == ORG_MODE )
      {
	 CurMode.c_iflag &= ~ICRNL;
	 CurMode.c_lflag &= ~(ICANON | ECHO);
	 CurMode.c_cc[VTIME] = 0;
	 CurMode.c_cc[VMIN] = 1;
	 ioctl( 0, TCSETS, &CurMode );
	 TermMode = GET_MODE;
      }
   else
      if ( TermMode == POLL_MODE )
	 {
	 CurMode.c_cc[VTIME] = 0;
	 CurMode.c_cc[VMIN] = 1;
	 ioctl( 0, TCSETS, &CurMode );
	 TermMode = GET_MODE;
	 }
   (void)read(0, &c, 1);

   return(c);
}


/*
 *   SpGetKey
 */

PUBLIC VOID SpGetkey()
{
   BUFFER_DECLARATIONS;
   BYTE c;

   DEBUG(( "SP.GETKEY {non-udp}" ));
   INIT_BUFFERS;

   c = GetAKey();

   DEBUG(("key was %c",c));
   PUT_BYTE( SP_SUCCESS );
   PUT_BYTE( c );  
   REPLY;
}

/*
 *   SpPollkey
 */

PUBLIC VOID SpPollkey()
{
   BUFFER_DECLARATIONS;
   char c;

   DEBUG(( "SP.POLLKEY" ));
   INIT_BUFFERS;

   if ( TermMode == ORG_MODE )
      {
	 CurMode.c_iflag &= ~ICRNL;
	 CurMode.c_lflag &= ~(ICANON | ECHO);
	 CurMode.c_cc[VTIME] = 1;
	 CurMode.c_cc[VMIN] = 0;
	 ioctl( 0, TCSETS, &CurMode );
	 TermMode = POLL_MODE;
      }
   else
      if ( TermMode == GET_MODE )
	 {
	    CurMode.c_cc[VTIME] = 1;
	    CurMode.c_cc[VMIN] = 0;
	    ioctl( 0, TCSETS, &CurMode );
	    TermMode = POLL_MODE;
	 }

   if ( read(0, &c, 1) == 0 )
      {
	 PUT_BYTE( SP_ERROR );
      }
   else
      {
	 PUT_BYTE( SP_SUCCESS );
	 PUT_BYTE( c );  
      }

   REPLY;

}


/*
 *   SpGetenv
 */

PUBLIC VOID SpGetenv()
{
   BUFFER_DECLARATIONS;
   BYTE *Name;

   DEBUG(( "SP.GETENV" ));
   INIT_BUFFERS;

   Name = &DataBuffer[0];
   GET_SLICE( Size, Name ); *(Name+Size)=0; DEBUG(( "\"%s\"", Name ));

   if( *Name == 0 )
      {
	 PUT_BYTE( SP_ERROR );
      }
   else
      {
	 if( ( Name=(BYTE *)getenv( Name ) ) == NULL )
	    {
	       PUT_BYTE( SP_ERROR );
	    }
	 else
	    {
	       DEBUG(( "\"%s\"", Name ));
	       PUT_BYTE( SP_SUCCESS );
	       Size = strlen( Name );
	       PUT_SLICE( Size, Name );
	    }
      }
   REPLY;
}

/*
 *   SpTime
 */

PUBLIC VOID SpTime()
{
   BUFFER_DECLARATIONS;
   long Time, UTCTime;

   DEBUG(( "SP.TIME" ));
   INIT_BUFFERS;

   tzset();
   time( &UTCTime );
   Time = UTCTime - timezone;
   PUT_BYTE( SP_SUCCESS );
   PUT_INT32( Time );
   PUT_INT32( UTCTime );
   REPLY;

#ifdef SUN
   UTCTime = time(NULL);
   Time = UTCTime + (localtime(&UTCTime))->tm_gmtoff;
   PUT_BYTE( SP_SUCCESS );
   PUT_INT32( Time );
   PUT_INT32( UTCTime );
   REPLY;
#endif


}

/*
 *   SpSystem
 */

PUBLIC VOID SpSystem()
{
   BUFFER_DECLARATIONS;
   BYTE *Command;
   INT32 Status=0L;
   BYTE  Result=SP_SUCCESS;

   DEBUG(( "SP.SYSTEM" ));
   INIT_BUFFERS;

   Command = &DataBuffer[0];
   GET_SLICE( Size, Command ); *(Command+Size)=0; DEBUG(( "\"%s\"", Command ));

   if (*Command == '\0')
   {
      Status = system("");
      if (Status)
	 Result = SP_ERROR;
   }
   else
      Status = system( Command );

   DEBUG(( "status %ld", Status ));
   PUT_BYTE( Result );
   PUT_INT32( Status );
   REPLY;
}

/*
 *   SpExit
 */

PUBLIC int SpExit()
{
   BUFFER_DECLARATIONS;
   long Status;
   
   DEBUG(( "SP.EXIT" ));
   INIT_BUFFERS;
   
   GET_INT32( Status );
   DEBUG(( "%ld", Status ));

   if( Status == 999999999 )
      Status = TERMINATE_OK_EXIT;
   else if( Status == -999999999 )
      Status = TERMINATE_FAIL_EXIT;

   DEBUG(( "exit with %d", (int)Status ));

   PUT_BYTE( SP_SUCCESS );
   REPLY ( (int)Status );
}

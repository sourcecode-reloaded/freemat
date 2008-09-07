/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "RPC.hpp"
#include "Class.hpp"
#include <math.h>

#ifdef USE_MPI
#include "MPIWrap.hpp"
#endif

void LoadCoreFunctions(Context* context) {
  // context->addFunction("tcpserver",TCPServerFunction,1,1,"port",NULL);
  // context->addFunction("tcpaccept",TCPAcceptFunction,2,1,"handle","timeout",NULL);
  // context->addFunction("tcpconnect",TCPConnectFunction,3,1,"remoteip","port","timeout",NULL);
  // context->addFunction("tcpsend",TCPSendFunction,3,1,"handle","data","timeout",NULL);
  // context->addFunction("tcprecv",TCPRecvFunction,2,1,"handle","timeout",NULL);
  // context->addFunction("tcpclose",TCPCloseFunction,1,0,"handle",NULL);
  // context->addFunction("tcpserverclose",TCPServerCloseFunction,1,0,"handle",NULL);
  // context->addFunction("tcpstate",TCPStateFunction,1,1,"handle",NULL);
  // context->addSpecialFunction("dbstep",DbStepFunction,1,0,"line",NULL);
  //context->addFunction("demo",DemoFunction,2,0,"x","string",NULL);
//   InitializeFileSubsystem();
  context->addFunction("eps",EpsFunction,1,1,"x",NULL);
  context->addSpecialFunction("blaslib",BlasLibraryFunction,1,0,"name",NULL);
#ifdef USE_MPI
  LoadMPIFunctions(context);
#endif
}

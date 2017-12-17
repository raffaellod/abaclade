﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2015-2017 Raffaello D. Di Napoli

This file is part of Lofty.

Lofty is free software: you can redistribute it and/or modify it under the terms of version 2.1 of the GNU
Lesser General Public License as published by the Free Software Foundation.

Lofty is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details.
------------------------------------------------------------------------------------------------------------*/

#include <lofty.hxx>
#include <lofty/app.hxx>
#include <lofty/coroutine.hxx>
#include <lofty/defer_to_scope_end.hxx>
#include <lofty/io/text.hxx>
#include <lofty/logging.hxx>
#include <lofty/thread.hxx>
#include <lofty/net/udp.hxx>

using namespace lofty;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class udp_echo_server_app : public app {
public:
   /*! Main function of the program.

   @param args
      Arguments that were provided to this program via command line.
   @return
      Return value of this program.
   */
   virtual int main(collections::vector<str> & args) override {
      LOFTY_TRACE_METHOD();

      LOFTY_UNUSED_ARG(args);
      // Schedule a UDP server. To connect to it, use: socat - UDP4:127.0.0.1:9081
      coroutine([this] () {
         LOFTY_TRACE_FUNC();

         net::ip::port port(9081);
         LOFTY_LOG(info, LOFTY_SL("server: starting, listening on port {}\n"), port);
         net::udp::server server(net::ip::address::any_v4, port);
         try {
            for (;;) {
               LOFTY_LOG(info, LOFTY_SL("server: waiting for datagrams\n"));
               // This will cause a context switch if no datagrams have yet been received.
               auto dgram(server.receive());

               LOFTY_LOG(info, LOFTY_SL("server: datagram received\n"));

               // Start a coroutine that will echo the datagram back to its sender.
               coroutine([&server, dgram] () {
                  LOFTY_TRACE_FUNC();

                  LOFTY_LOG(
                     info, LOFTY_SL("responder: starting for {}:{}\n"), dgram->address(), dgram->port()
                  );

                  // Send the datagram back as a reply.
                  server.send(dgram);

                  LOFTY_LOG(info, LOFTY_SL("responder: terminating\n"));
               });
            }
         } catch (execution_interruption const &) {
            LOFTY_LOG(info, LOFTY_SL("server: terminating\n"));
            // Rethrow the exception to ensure that all remaining coroutines are terminated.
            throw;
         }
      });

      // Switch this thread to run coroutines, until they all terminate.
      this_thread::run_coroutines();
      // Execution resumes here, after all coroutines have terminated.
      LOFTY_LOG(info, LOFTY_SL("main: terminating\n"));
      return 0;
   }
};

LOFTY_APP_CLASS(udp_echo_server_app)

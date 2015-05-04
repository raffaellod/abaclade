﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2015
Raffaello D. Di Napoli

This file is part of Abaclade.

Abaclade is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Abaclade is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with Abaclade. If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------*/

#include <abaclade.hxx>
#include <abaclade/collections/map.hxx>
#include <abaclade/thread.hxx>

#include <atomic>


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::thread::comm_manager

namespace abc {

class thread::comm_manager : public noncopyable {
public:
   //! Constructor.
   comm_manager();

   //! Destructor.
   ~comm_manager();

   /*! Returns a pointer to the singleton instance.

   @return
      Pointer to the only instance of this class.
   */
   static comm_manager * instance() {
      return sm_pInst;
   }

#if ABC_HOST_API_POSIX
   /*! Returns the signal number to be used to inject the specified type of exception in a thread.

   @param inj
      Type of injectable exception.
   @param return
      Signal number to use.
   */
   int injectable_exception_signal_number(exception::injectable inj) const;
#endif

   /*! Registers the termination of the program’s abc::app::main() overload.

   @param inj
      Type of exception that escaped the program’s main(), or exception::injectable::none if main()
      returned normally.
   */
   void main_thread_terminated(exception::injectable inj);

   /*! Registers a new thread as running.

   @param pimpl
      Pointer to the abc::thread::impl instance running the thread.
   */
   void nonmain_thread_started(std::shared_ptr<impl> const & pimpl);

   /*! Registers a non-main thread as no longer running.

   @param pimpl
      Pointer to the abc::thread::impl instance running the thread.
   @param bUncaughtException
      true if an exception escaped the thread’s function and was only blocked by thread:impl.
   */
   void nonmain_thread_terminated(impl * pimpl, bool bUncaughtException);

private:
#if ABC_HOST_API_POSIX
   /*! Handles SIGINT and SIGTERM for the main thread, as well as the Abaclade-defined signal used
   to interrupt any thread, injecting an appropriate exception type in the thread’s context.

   @param iSignal
      Signal number for which the function is being called.
   @param psi
      Additional information on the signal.
   @param pctx
      Thread context. This is used to manipulate the stack of the thread to inject a call frame.
   */
   static void interruption_signal_handler(int iSignal, ::siginfo_t * psi, void * pctx);
#endif

public:
#if ABC_HOST_API_POSIX
   //! Signal number to be used to interrupt threads.
   int const mc_iInterruptionSignal;
#endif
   /*! Pointer to an incomplete abc::thread::impl instance that’s used to control the main (default)
   thread of the process. */
   // TODO: instantiate this lazily, only if needed.
   std::shared_ptr<impl> m_pimplMainThread;
   //! Tracks all threads running in the process except *m_pimplMainThread.
   // TODO: make this a set instead of a map.
   collections::map<impl *, std::shared_ptr<impl>> m_mappimplThreads;
   //! true if the main thread of the process is terminating, or false otherwise.
   std::atomic<bool> m_bMainThreadTerminating;
   //! Pointer to the singleton instance.
   static thread::comm_manager * sm_pInst;
};

} //namespace abc

////////////////////////////////////////////////////////////////////////////////////////////////////
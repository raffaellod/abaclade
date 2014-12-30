﻿/* -*- coding: utf-8; mode: c++; tab-width: 3; indent-tabs-mode: nil -*-

Copyright 2010, 2011, 2012, 2013, 2014
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

// #include <abaclade.hxx> already done in exception-os.cxx.

#include <cstdlib> // std::abort()
#include <signal.h> // sigaction sig*()
#include <ucontext.h> // ucontext_t


////////////////////////////////////////////////////////////////////////////////////////////////////
// abc::exception

namespace {

//! Signals that we can translate into C++ exceptions.
int const g_aiHandledSignals[] = {
// Signal (Default action) Description (standard).
// SIGABRT, // (Core) Abort signal from abort(3) (POSIX.1-1990).
// SIGALRM, // (Term) Timer signal from alarm(2) (POSIX.1-1990).
   SIGBUS,  // (Core) Bus error (bad memory access) (POSIX.1-2001).
// SIGCHLD, // (Ign ) Child stopped or terminated (POSIX.1-1990).
// SIGCONT, // (Cont) Continue if stopped (POSIX.1-1990).
   SIGFPE,  // (Core) Floating point exception (POSIX.1-1990).
// SIGHUP,  // (Term) Hangup on controlling terminal or death of controlling process (POSIX.1-1990).
// SIGILL,  // (Core) Illegal Instruction (POSIX.1-1990).
// SIGINT,  // (Term) Interrupt from keyboard (POSIX.1-1990).
// SIGPIPE, // (Term) Broken pipe: write to pipe with no readers (POSIX.1-1990).
// SIGPROF, // (Term) Profiling timer expired (POSIX.1-2001).
// SIGQUIT, // (Core) Quit from keyboard (POSIX.1-1990).
   SIGSEGV  // (Core) Invalid memory reference (POSIX.1-1990).
// SIGTERM  // (Term) Termination signal (POSIX.1-1990).
// SIGTRAP  // (Core) Trace/breakpoint trap (POSIX.1-2001).
// SIGTSTP  // (Stop) Stop typed at terminal (POSIX.1-1990).
// SIGTTIN  // (Stop) Terminal input for background process (POSIX.1-1990).
// SIGTTOU  // (Stop) Terminal output for background process (POSIX.1-1990).
// SIGUSR1  // (Term) User-defined signal 1 (POSIX.1-1990).
// SIGUSR2  // (Term) User-defined signal 2 (POSIX.1-1990).
};
//! Default handler for each of the signals above.
struct ::sigaction g_asaDefault[ABC_COUNTOF(g_aiHandledSignals)];

//! Possible exception types thrown by throw_after_fault().
ABC_ENUM_AUTO_VALUES(fault_exception_types,
   arithmetic_error,
   division_by_zero_error,
   floating_point_error,
   memory_access_error,
   memory_address_error,
   null_pointer_error,
   overflow_error
);

/*! Throws an exception of the specified type.

@param fxt
   Type of exception to be throw.
@param iArg0
   Exception type-specific argument 0.
@param iArg1
   Exception type-specific argument 1.
*/
void throw_after_fault(
   fault_exception_types::enum_type fxt, std::intptr_t iArg0, std::intptr_t iArg1
) {
//printf("throw_after_fault: %d %p %p\n", fxt, iArg0, iArg1);
   ABC_UNUSED_ARG(iArg1);
   switch (fxt) {
      case fault_exception_types::arithmetic_error:
         ABC_THROW(abc::arithmetic_error, ());
      case fault_exception_types::division_by_zero_error:
         ABC_THROW(abc::division_by_zero_error, ());
      case fault_exception_types::floating_point_error:
         ABC_THROW(abc::floating_point_error, ());
      case fault_exception_types::memory_access_error:
         ABC_THROW(abc::memory_access_error, (reinterpret_cast<void const *>(iArg0)));
      case fault_exception_types::memory_address_error:
         ABC_THROW(abc::memory_address_error, (reinterpret_cast<void const *>(iArg0)));
      case fault_exception_types::null_pointer_error:
         ABC_THROW(abc::null_pointer_error, ());
      case fault_exception_types::overflow_error:
         ABC_THROW(abc::overflow_error, ());
      default:
         // Unexpected exception type. Should never happen.
         std::abort();
   }
}

/*! Translates POSIX signals into C++ exceptions, whenever possible. This works by injecting the
stack frame of a call to throw_after_fault(), and then returning, ending processing of the signal.
Execution will resume from throw_after_fault(), which creates the appearance of a C++ exception
being thrown at the location of the offending instruction, without calling any of the (many)
functions that are forbidden in a signal handler.

@param iSignal
   Signal number for which the function is being called.
@param psi
   Additional information on the signal.
@param pctx
   Thread context. This is used to manipulate the stack of the thread to inject a call frame.
*/
void fault_signal_handler(int iSignal, ::siginfo_t * psi, void * pctx) {
   /* Don’t let external programs mess with us: if the source is not the kernel, ignore the error.
   POSIX.1-2008 states that:
      “Historically, an si_code value of less than or equal to zero indicated that the signal was
      generated by a process via the kill() function, and values of si_code that provided additional
      information for implementation-generated signals, such as SIGFPE or SIGSEGV, were all
      positive. […] if si_code is less than or equal to zero, the signal was generated by a process.
      However, since POSIX.1b did not specify that SI_USER (or SI_QUEUE) had a value less than or
      equal to zero, it is not true that when the signal is generated by a process, the value of
      si_code will always be less than or equal to zero. XSI applications should check whether
      si_code is SI_USER or SI_QUEUE in addition to checking whether it is less than or equal to
      zero.”
   So we do exactly that – except we skip checking for SI_USER and SI_QUEUE at this point because
   they don’t apply to many signals this handler takes care of. */
   if (psi->si_code <= 0) {
      return;
   }

   fault_exception_types::enum_type fxt;
   std::intptr_t iArg0 = 0, iArg1 = 0;
   switch (iSignal) {
      case SIGBUS:
         /* There aren’t many codes here that are safe to handle; most of them indicate that there
         is some major memory corruption going on, and in that case we really don’t want to keep on
         going – even the code to throw an exception could be compromised. */
         switch (psi->si_code) {
            case BUS_ADRALN: // Invalid address alignment.
               fxt = fault_exception_types::memory_access_error;
               iArg0 = reinterpret_cast<std::intptr_t>(psi->si_addr);
               break;
            default:
               std::abort();
         }
         break;

      case SIGFPE:
         switch (psi->si_code) {
            case FPE_INTDIV: // Integer divide by zero.
               fxt = fault_exception_types::division_by_zero_error;
               break;
            case FPE_INTOVF: // Integer overflow.
               fxt = fault_exception_types::overflow_error;
               break;
            case FPE_FLTDIV: // Floating-point divide by zero.
            case FPE_FLTOVF: // Floating-point overflow.
            case FPE_FLTUND: // Floating-point underflow.
            case FPE_FLTRES: // Floating-point inexact result.
            case FPE_FLTINV: // Floating-point invalid operation.
            case FPE_FLTSUB: // Subscript out of range.
               fxt = fault_exception_types::floating_point_error;
               break;
            default:
               /* At the time of writing, the above case labels don’t leave out any values, but
               that’s not necessarily going to be true in 5 years, so… */
               fxt = fault_exception_types::arithmetic_error;
               break;
         }
         break;

      case SIGSEGV:
         if (psi->si_addr == nullptr) {
            fxt = fault_exception_types::null_pointer_error;
         } else {
            fxt = fault_exception_types::memory_address_error;
            iArg0 = reinterpret_cast<std::intptr_t>(psi->si_addr);
         }
         break;

      default:
         /* Handle all unrecognized cases here. Since here we only handle signals for which the
         default actions is a core dump, calling abort (which sends SIGABRT, also causing a core
         dump) is the same as invoking the default action. */
         std::abort();
   }

   /* Change the address at which the thread will resume execution: manipulate the thread context
   to emulate a function call to throw_after_fault(). */

   ::ucontext_t * puctx = static_cast< ::ucontext_t *>(pctx);
   #if ABC_HOST_ARCH_I386
      #if ABC_HOST_API_LINUX
         typedef std::int32_t reg_t;
         reg_t & eip = puctx->uc_mcontext.gregs[REG_EIP];
         reg_t & esp = puctx->uc_mcontext.gregs[REG_ESP];
      #elif ABC_HOST_API_FREEBSD
         typedef std::int32_t reg_t;
         reg_t & eip = puctx->uc_mcontext.mc_eip;
         reg_t & esp = puctx->uc_mcontext.mc_esp;
      #else
         #error "TODO: HOST_API"
      #endif
      /* Push the arguments to throw_after_fault() onto the stack, push the address of the current
      (failing) instruction, then set eip to the start of throw_after_fault(). These steps emulate a
      3-argument subroutine call. */
      reinterpret_cast<reg_t *>(esp -= 4) = static_cast<reg_t>(iArg1);
      reinterpret_cast<reg_t *>(esp -= 4) = static_cast<reg_t>(iArg0);
      reinterpret_cast<reg_t *>(esp -= 4) = static_cast<reg_t>(fxt);
      reinterpret_cast<reg_t *>(esp -= 4) = eip;
      eip = reinterpret_cast<reg_t>(&throw_after_fault);
   #elif ABC_HOST_ARCH_X86_64
      #if ABC_HOST_API_LINUX
         typedef std::int64_t reg_t;
         reg_t & rip = puctx->uc_mcontext.gregs[REG_RIP];
         reg_t & rsp = puctx->uc_mcontext.gregs[REG_RSP];
         reg_t & rdi = puctx->uc_mcontext.gregs[REG_RDI];
         reg_t & rsi = puctx->uc_mcontext.gregs[REG_RSI];
         reg_t & rdx = puctx->uc_mcontext.gregs[REG_RDX];
      #elif ABC_HOST_API_FREEBSD
         typedef std::int64_t reg_t;
         reg_t & rip = puctx->uc_mcontext.mc_rip;
         reg_t & rsp = puctx->uc_mcontext.mc_rsp;
         reg_t & rdi = puctx->uc_mcontext.mc_rdi;
         reg_t & rsi = puctx->uc_mcontext.mc_rsi;
         reg_t & rdx = puctx->uc_mcontext.mc_rdx;
      #else
         #error "TODO: HOST_API"
      #endif
      /* Load the arguments to throw_after_fault() in rdi/rsi/rdx, push the address of the current
      (failing) instruction, then set rip to the start of throw_after_fault(). These steps emulate a
      3-argument subroutine call. */
      rdi = static_cast<reg_t>(fxt);
      rsi = static_cast<reg_t>(iArg0);
      rdx = static_cast<reg_t>(iArg1);
      // TODO: validate that stack alignment to 16 bytes is done by the callee with push rbp.
      *reinterpret_cast<reg_t *>(rsp -= 8) = rip;
      rip = reinterpret_cast<reg_t>(&throw_after_fault);
   #else
      #error "TODO: HOST_ARCH"
   #endif
}

} //namespace

namespace abc {

exception::fault_converter::fault_converter() {
   // Setup handlers for the signals in g_aiHandledSignals.
   struct ::sigaction saNew;
   saNew.sa_sigaction = &fault_signal_handler;
   sigemptyset(&saNew.sa_mask);
   /* SA_SIGINFO (POSIX.1-2001) provides the handler with more information about the signal, which
   we use to generate more precise exceptions. */
   saNew.sa_flags = SA_SIGINFO;
   for (std::size_t i = ABC_COUNTOF(g_aiHandledSignals); i-- > 0; ) {
      ::sigaction(g_aiHandledSignals[i], &saNew, &g_asaDefault[i]);
   }
}

exception::fault_converter::~fault_converter() {
   // Restore the saved signal handlers.
   for (std::size_t i = ABC_COUNTOF(g_aiHandledSignals); i-- > 0; ) {
      ::sigaction(g_aiHandledSignals[i], &g_asaDefault[i], nullptr);
   }
}

} //namespace abc

////////////////////////////////////////////////////////////////////////////////////////////////////

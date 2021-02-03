# CSP
 CSP support C++ template library

 Article: Implementation of the CSP Semantics of Inter-Process Communications using the C++11 Standard Library.
 http://conf.uni-ruse.bg/bg/docs/cp20/bp/bp-4.pdf

 Folder 0005 contains CSP library's sample subprojects:
 - 0002 Producer-Customer Type 1:1
 - 0004 Producer-Buffer-Customer Type 1:1:1
 - 0007 Producer-Customer Type 2:1 (Non-determinism)
 - 0008 Complex Communication Scheme (CCS)
 - 0010 Complex Communication Scheme with Unification (CCSU)
 - 0011 Output Fork Testbed
 - 0012 Output Fork-Input Mux Testbed
 - 0013 Alternate access to shared object (AASHO) - could be considered as example of 2 coprocedures system
 - 0014 Coprocedures (CoP)
 - 0015 Distributed Mutual Exclusion with Central Server (DME) - Var1: Centralized control of coprocedures (Strong predefined sequential dispatching)
 - 0018 Sink type input (n:1/n)
 - 0019 Sink-Mux Exemplary System
 - 0023 Mailbox/Active Mailbox
 - 0024 Distributed Mutual Exclusion with Central Server (DME) - Var2: Non-deterministic dispatching

 Subprojects have two editions:
 - traditional with explicit threading;
 - with Actors.

 Applications used Embarcadero C++ Builder but are portable to other development platforms.

 The library itself is the file csp.h which use only C++11 standard features and thus is portable without any change.

 Other sample subprojects are on the go...

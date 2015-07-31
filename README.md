The project is migrated from [https://code.google.com/p/fwprofile/](https://code.google.com/p/fwprofile/).

# FW Profile
C Implementation of an unambiguous definition of State Machines and Activity Diagrams for Safety-Critical, Real-Time and Embedded Applications.

Its main features are:

* **Well-Defined Semantics**: clearly and unambiguously defined behaviour.
* **Minimal Memory Requirements**: core module footprint of a few kBytes.
* **Small CPU Demands**: one single level of indirection (due to actions and guards being implemented as function pointers).
* **Excellent Scalability**: memory footprint and CPU demands are independent of number and size of state machine and procedure instances.
* **High Reliability**: test suite with 100% code, branch, and condition coverage (excluding error branches for system calls).
* **Formal Specification**: user requirements formally specify the implementation.
* **Requirement Traceability**: all requirements are individually traced to their implementation and to verification evidence.
* **Documented Code**: doxygen documentation for all the source code.
* **Demo Application**: complete application demonstrating capabilities and mode of use.
* **Support for Extensibility**: an inheritance-like mechanism is provided through which a derived state machine or a derived procedure is created from a base state machine or base procedure by overriding some of its actions or guards.
* **Processor Architecture Independence**: independent of processor architecture, suitable for 8, 16, 32, 64-bit systems.
* **Simple programming interface**: allowing fast manual definition of state machines or activity diagrams directly at the C code level, or development of a code generator for a UML modeling tool.

# Web-Based Modeling Tool and Code Generator
A web-based modeling tool is freely available to help design FW Profile state machines and procedures, and to automate the generation of the C-code which configures them. The tool can be accessed from [here](http://pnp-software.com/fwprofile/editor/), the most recent version is **4.5**.

FW Profile Editor is also available on Chrome Web Store. As an app for Chrome allows you to work offline. Search for FW Profile at Chrome Web Store, or try this link: https://chrome.google.com/webstore/search/FW%20Profile

# Ownership
The owner of the project is [P&P Software GmbH](http://pnp-software.com/).

# License
Free use of this software is granted under the terms of the GNU Lesser General Public License (LGPL) v3, see LICENSE.

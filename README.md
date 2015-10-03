
An Automated toolkit for Receiver Function processing
=====================================================
#1. Hello world!
To survive the big seismic data avalanche, an high-level automated standard is indispensible. This is the primary reason for the birth of this toolkit. All the standards and their realization by some kind of programming language are built step by step, thus modification is inevitable and everything will be out of date from the long view. The built in philosophy of this toolkit are "concise", "effective" and "application-oriented".

#2. Tools
##2.1 pickRf
Determine an RF is good or bad according to obvious criterias, including "being postive around ZERO", "P phase must be dominant", and so on.
##2.2 pickRf_WV
Given a number of RFs and a reference RF, determine which is good or bad according to their similarity to the reference RF. The reference RF comes from input or just the stacking result of all the RFs.

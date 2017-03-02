
Automated Toolkit for Picking Receiver Functions
================================================
#1. Target
It is really boring for manually picking receiver functions. Empericially determine which is high-quality and which is not, is really non-efficient, and may introduce artificial error into picking routine.

This package aims to digitalize some picking criteria, thus machine could replace man's jobs.

#2. Tools
- `pickRf` etermine an RF is good or bad according to obvious criterias, including "being postive around ZERO", "P phase must be dominant", and so on.
- `pickRf_WV` given a reference RF, determine which is good or bad according to their similarity to the reference RF. The reference RF comes from input or just the stacking result of all RFs.

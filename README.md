PascalScheduler
================

## Disclamer.
This project is for (self) educational purpose.
Still if you have any suggestion on what to do next, or on how to do better, submit an issue.

## Origin of the name.
Pascal is obviously a reference to [Blaise Pascal](wikipedia.org/wiki/Blaise_Pascal).
I just didn't found a better name, so here it is.

## What is this?
This is a really simple [Round Robin Scheduler](wikipedia.org/wiki/Round-robin_scheduling).
A timer regularly(every 1ms for now) start a scheduling function. This function save the current task state and restore the next task state.

A sleep function is available. You can provide the number of scheduler tick before task wake-up. This number guarantee a minimal value. The maximal value should be this number plus the number of running tasks.

## Why a RR Scheduler?

### RR Scheduler vs _big loop_.
Once the scheduler is available, it is as easy to write a task for it than to call a function from a _big loop_.
The _big loop_ approche may seems cool and easy to use, but how do you mix blocking functions and evenmential programming?
Having two distinct tasks for this and using IPC is an elegant way to solve the problem, and using RRScheduler is probably the easiest way to implement tasks.

### RR Scheduler vs Other Scheduler.
Priority based scheduler or any other scheduler may be a good idea, but RR Scheduler is easy to implement(you don't need any queue, just a simple array) and starvation free.

### Is this a true RR Scheduler?
A task can release the CPU, or it can be blocked by an IPC.
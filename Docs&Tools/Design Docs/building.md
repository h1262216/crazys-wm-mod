# Building Processing
The week-processing of a building runs as follows:

First, the building-wide `BeginWeek` function is executed. This function resets the weekly statistics,
and does some basic girl updates including removing dead ones. Specific buildings can extend is 
function by providing `onBeginWeek`. For any non-dead girl, the `GirlBeginWeek` function handles the
corresponding updates.

Then the two shifts will be run using the `RunShift` function. A shift runs in three phases:
* `BeginShift`
* `UpdateGirls`
* `EndShift`

During `BeginShift`, the buildings resources are set up, all girls' `GirlBeginShift` is called, and
we figure out if there is a Matron working this shift as well as handle any resting jobs. Each buildings
resources and initial customers are set up in this phase as well.

The `GirlBeginShift` does some general updates to the girl, and then calls the `handle_pre_shift` handler of
the girl's current job.



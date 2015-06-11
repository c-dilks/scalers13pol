1. Execute `GetRunList` to get a list of runs which contain `$triggerconf` in the
   name of the trigger configuration; the output file is `culled_run_list.txt` with
   columns `trigger name` `run number` `fill number`

2. Execute `BuildHPSSlist` to build files `files_to_retrieve.[listnum].lst` for all runs 
   listed in `culled_run_list.txt` which have run number greater than `$(cat lastrun)`;
   `listnum` is an iterator for each list.. the number of entries per list is defined
   by `$size` in `BuildHPSSlist`

3. Check `sca2015` directory and clear any old `*.dat` files (move them to `transferred`)

4. Execute `Submit [listnum]` to submit `files_to_retrieve.[listnum].lst` to the HPSS
   queue; it will then change `lastrun` to the last run on this list and move the list file
   to the `submitted` directory along with a unix timestamp

5. Monitor progress with `hpss_user.pl -w | grep $(whoami)`

# Process Scheduler

## Contributers

The following students contributed to this project:
* Kyle Martinez
* Joshua Hack
* Samuel Lewis

## Building

To run, set your working directory to the root of the project folder then
execute `make`. This will generate an executable binary with the
filepath `./bin/scheduler`.

## Testing

Testing can be done by running `python tests/acceptance.py` in the root of the
project folder. A report containing the number of passed acceptance tests
will be printed to stdout. Should compilation of the scheduler fail, the script
will notify the user.

## Input Format

The scheduler program requires that a file, called `processes.in`, be present
in the current working directory. This file must conform to the following
format:

```
processcount 2        # Read 5 processes
runfor 15             # Run for 15 time units
use rr                # Can be fcfs, sjf, or rr
quantum 2             # Time quantum – only if using rr
process name P1 arrival 3 burst
process name P2 arrival 0 burst 9
end
```

As you can see, lines can include comments! Simply prefix your comment with
a hashtag (i.e. #). All text after the hashtag will be ignored. Although the
quantum is only used by the round-robin scheduler, it is advised that the
quantum line simply be commented out when not in use.

## Usage

After building the executable binary (see [Building](#building)), and including
a file called `processes.in` in your current working directory
(see [Input Format](#input-format)), you can run the program simply by executing
`./bin/scheduler` from the command line. For illustrative purposes:

```
~/Code/process-scheduler master*
❯ ls -a
.               .clang_complete .gitignore      bin     makefile  src
..              .git            README.md       include         processes.in    test

~/Code/process-scheduler master*
❯ make
mkdir -p bin
gcc -std=gnu99 src/* -I include -o ./bin/scheduler

~/Code/process-scheduler master*
❯ ./bin/scheduler
```

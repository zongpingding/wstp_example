## Prerequisite
Here are some notes:

1. For Arch user, install `mathematica` or `mathematica-light`.
2. Check if your shell is `zshell`.
2. Change the `WOLFRAM` variable in `run.sh`.


## Build
### wolfram repl
Build it with command:

``` shell
# build
./run.sh wstp_repl.c

# after building, simply run
./wstp_repl
```


### wolfram daemon
Commands list as follows, take `wstp_type=NET` as example:

```shell
# launch server(wolfram kernel)
wolfram -script server.wls

# build client
./run.sh wstp_daemon.c

# after building, simply run 
./wstp_daemon "2^64"
./wstp_daemon "Integrate[Sin[x], {x, 0, 2/3*Pi}]"
```


## Reference
Some materials about WSTP:

* [WSTP and External Program Communication Overview](https://reference.wolfram.com/language/tutorial/WSTPAndExternalProgramCommunicationOverview.html)
* [Using WSTP to Communicate between Wolfram System Sessions](https://reference.wolfram.com/language/tutorial/UsingWSTPToCommunicateBetweenWolframSystemSessions.html)
* [Running the Wolfram System from within an External Program](https://reference.wolfram.com/language/tutorial/RunningTheWolframSystemFromWithinAnExternalProgram.html)

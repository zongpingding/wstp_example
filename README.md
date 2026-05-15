## Prerequisite
Here are some notes:

1. For Arch user, install `mathematica` or `mathematica-light`.
2. Check if your shell is `zshell`.
2. Change the `WOLFRAM` and `WSTP_PATH` variable in `run.sh`.


## Build
### wolfram repl
Build it with command:

``` shell
# build
./run.sh wstp_repl.c

# after building, simply run
./wstp_repl
```


### wolfram server & client
Commands list as follows, take `wstp_type=NET` as example:

```shell
# launch server(wolfram kernel)
wolfram -script wstp_server.wls

# build & run client
./run.sh wstp_client.c

# after building, simply run 
./wstp_client "2^64"
./wstp_client 'num1=2^64; Print["Hello From wstp client"]; num1'
./wstp_client '<< "example.wls"'
./wstp 'Integrate[Sin[x]/x^3, x]'
./wstp_client "Integrate[Sin[x], {x, 0, 2/3*Pi}]//TeXForm//ToString"
```

File `example.wls` contains just 3 lines:

```wolfram
(* This is file 'example.wls' *)
TeXResult=ToString[TeXForm[LaplaceTransform[t^4 Sin[3*t], t, s]]];
Export["output.txt",TeXResult]
```

> WARN: `wstp` will only print the last experssion that does not followed by a semicolon in `wstp '<< "<file>"'` syntax.


### image support(experiment)
Currently, `wstp_client` only has a limited support for image generating(export), check [wstp_client_img.c](experiment/wstp_client_img.c) for details.

A simple example:

``` shell
./wstp_client -i "Plot[Sin[x], {x, 0, 2*Pi}]"
./wstp_client -i "Plot[Sin[x], {x, 0, 2*Pi}]" -o 'test.png'
```

In the first case, you'll see a figure named `output.png` appears in current directory.

> WARN: do NOT support exporting pictures in vector format, such as `PDF`, `SVG` etc.


## Usage
This repo provides some scripts(config) to make `wstp_client` and `wstp_server` work with other programs:

* Zsh(Bash): check [README](scripts/README.md)
* Emacs: check [README](emacs_integration/README.md)


## Reference
Some materials about WSTP:

* [WSTP and External Program Communication Overview](https://reference.wolfram.com/language/tutorial/WSTPAndExternalProgramCommunicationOverview.html)
* [Using WSTP to Communicate between Wolfram System Sessions](https://reference.wolfram.com/language/tutorial/UsingWSTPToCommunicateBetweenWolframSystemSessions.html)
* [Running the Wolfram System from within an External Program](https://reference.wolfram.com/language/tutorial/RunningTheWolframSystemFromWithinAnExternalProgram.html)
* [WSTP Packets](https://reference.wolfram.com/language/guide/WSTPPackets.html)

Deeply appreciate the innovative tools provided by [Wolfram Research](https://www.wolfram.com/).

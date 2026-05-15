## Launch
Copy `wstp_server.wls` to a location of your choice, and change it in `wstp_daemon`. Add scripts - `wstp` and `wstp_daemon` to your `PATH`, then launch the kernel(daemon):

```shell
wstp_daemon
```


## Examples
Test script as follows:

```wolfram
(* This is file 'wstp_script_test.wls' *)
TeXResult=ToString[TeXForm[LaplaceTransform[t^4 Sin[3*t], t, s]]];
Export["output.txt",TeXResult]
```


Basic usage:

```shell
# support 'Print' & variable print
$ wstp 'Print["Hello wstp"]'
Hello wstp
$ wstp 'num1=2^64; Print["Hello From wstp client"]; num1' 
Hello From wstp client
18446744073709551616


# catch runtime error
$ wstp 'Sin[]'
Sin::argx: Sin called with 0 arguments; 1 argument is expected.$Failed


# simple text format
$ wstp 'Integrate[Sin[x]/x^3, x]'
  1  Cos[x]   Sin[x]   SinIntegral[x]
-(-) ------ - ------ - --------------
  2    x          2          2
               2 x

# common usage 
$ wstp "1+1"
2
$ wstp 'Integrate[x^2+1/(x^3-1), x]//TeXForm//ToString'
\frac{x^3}{3}-\frac{1}{6} \log \left(x^2+x+1\right)+\frac{1}{3} \log (1-x)-\frac{\tan ^{-1}\left(\frac{2 x+1}{\sqrt{3}}\right)}{\sqrt{3}}


# run script
$ wstp '<< "wstp_script_test.wls"'
output.txt
```

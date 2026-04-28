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
# no return value
$ wstp 'Print["Hello wstp"]'
Null


# common usage 
$ wstp "1+1"
2
$ wstp 'Integrate[Sin[x]/x^3, x]'
Failed to get result(string)
$ wstp 'Integrate[x^2+1/(x^3-1), x]//TeXForm//ToString'
\frac{x^3}{3}-\frac{1}{6} \log \left(x^2+x+1\right)+\frac{1}{3} \log (1-x)-\frac{\tan ^{-1}\left(\frac{2 x+1}{\sqrt{3}}\right)}{\sqrt{3}}


# run script
$ wstp '<< "wstp_script_test.wls"'
output.txt
```

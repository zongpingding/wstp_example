(* This is file 'wstp_script_test.wls' *)
TeXResult=ToString[TeXForm[LaplaceTransform[t^4 Sin[3*t], t, s]]];
Export["output.txt",TeXResult]

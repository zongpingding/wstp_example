(* This is file 'example.wls' *)
TeXResult=ToString[TeXForm[LaplaceTransform[t^4 Sin[3*t], t, s]]];
Export["output.txt",TeXResult]

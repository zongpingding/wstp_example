(* this is file 'emacs_example.wls' *)


(* load MaTeX for LaTeX support *)
(* <<MaTeX` *)


(* get intersections *)
lAux = InfiniteLine[{{10, 0}, {0, 10}}];
coors = RegionIntersection[lAux, Circle[{5, 5}, 2]];
(* Print[coors] *)

(* 2d intersection region *)
boolReg = RegionIntersection[
    Disk[{5, 5}, 2],
    Polygon[{{5, 5}, {0, 10}, {10, 10}}]];

tikzlikefigMeta = Graphics[{
  (* Rectangle[{0, 0}, {10, 10}], *)
  Line[{{0, 0}, {10, 10}}],
  { Arrowheads[{-0.02, 0.02}], Arrow[{{6, 1.5}, {8, 1}, {9, 5}}]},
  { Dashed, lAux },
  Circle[{5, 5}, 2],
  {Red, AbsolutePointSize[5], Point[{5, 5}]},
  {
   FaceForm[LightBlue],
   EdgeForm[Directive[Dashed, Blue]],
   Polygon[{{2, 2}, {3, 5}, {2, 4}}]
  },
  Polygon[{{1+8, 0+5}, {0+8, Sqrt[3]+5}, {-1+8, 0+5}},
    VertexColors -> {Red, Green, Blue}],

  (* Plot Intersections *)
  {Blue, AbsolutePointSize[6], coors},

  (* add text *)
  Text["Sample Text", {5, 6}, FormatType->StandardForm],
  Text["Equation: a^2+b^2=c^2", {5, 9},
    Background->LightRed,
    FormatType->TraditionalForm],
  (* MaTeX[]//Head --> Graphics *)
  Inset[MaTeX["\\fbox{$a^2=c^2$}"], {3, 3}, {Left, Top}]
}
  (* Axes -> True, *)
  (* AxesStyle -> Arrowheads[0.02] *)
]

tikzlikefigCompound1 = Region[
  Style[boolReg, Orange, Opacity[0.7]]
];

tikzlikefigCompound2 = Plot[
  Sin[x], {x, 0, 3*Pi},
  PlotStyle->{AbsoluteThickness[1]}, Axes -> False,
  ColorFunction -> Function[{x, y},
  ColorData["NeonColors"][0.5*y]]
];

CoolColor[ z_ ] := RGBColor[z, 1 - z, 1];
tikzlikefigCompound3 = DensityPlot[
    Sin[x y], {x, 4, 6}, {y, 0.5, 2.5},
    ColorFunction -> CoolColor
]

tikzlikefig = Show[
  tikzlikefigMeta,
  tikzlikefigCompound1,
  tikzlikefigCompound2,
  tikzlikefigCompound3,
  PlotRange  -> All,
  Axes       -> True,
  GridLines  -> {Table[x, {x, 0, 9.5, 0.5}], Table[y, {y, -1, 9.5, 0.5}]},
  AxesStyle  -> Arrowheads[0.02],
  TicksStyle -> Directive[FontFamily->"Times New Roman"]
]


Export["emacs_example.pdf", tikzlikefig]

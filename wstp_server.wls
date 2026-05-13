(* wstp_server.wls *)
(* client is written in C *)


$HistoryLength = 0; 
port = "5000";

While[True,
    Print["[Server] Waiting for Client to connect on port ", port, "..."];
    link = Quiet@LinkOpen[port, LinkMode -> Listen, LinkProtocol -> "TCPIP"];
    If[link === $Failed, Pause[2]; Continue[];];

    Quiet[
        connected = False;
        While[True,
            packet = LinkRead[link, Hold];
            If[packet === $Failed || packet === Hold[$Failed], Break[]];
            If[!connected,
                Print["[Server] Client connected!"];
                connected = True;
            ];

            If[MatchQ[packet, Hold[EvaluatePacket[_]]],
                exprHold = Extract[packet, {1, 1}, Hold];
                Block[
                    { $ParentLink = link }, 
                    result = Check[ReleaseHold[exprHold], $Failed];
                    LinkWrite[link, ReturnPacket[result]];
                ];
            ,
                LinkWrite[link, ReturnPacket[Null]];
            ];
        ];
    , {LinkObject::linkd, LinkObject::linkx}
    ];

    If[connected,
        Print["[Server] Client disconnected."];
    ];
    Quiet@LinkClose[link];
]

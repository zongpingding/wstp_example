(* server.wls *)
(* client is written in C *)


$HistoryLength = 0; 
port = "5000";

Print["[Server] Starting WSTP Server on TCP port ", port, "..."];

While[True,
    Print["[Server] Waiting for C Client to connect on port ", port, "..."];

    (* listen *)
    link = Quiet@LinkOpen[port, LinkMode -> Listen, LinkProtocol -> "TCPIP"];
    If[link === $Failed,
        Print["[Server] Failed to open port. Retrying in 2 seconds..."];
        Pause[2];
        Continue[];
    ];

    (* wait *)
    packet = LinkRead[link];
    If[packet === $Failed,
        Quiet@LinkClose[link];
        Continue[];
    ];
    Print["[Server] Client connected!"];
    
    (* loop *)
    While[packet =!= $Failed,
        If[Head[packet] === EvaluatePacket,
            expr = First[packet];
            result = Check[Evaluate[expr], $Failed];
            LinkWrite[link, ReturnPacket[result]];
        ,
            LinkWrite[link, ReturnPacket[Null]];
        ];
        packet = LinkRead[link];
    ];
   
    (* The end *)
    Print["[Server] Client disconnected."];
    Quiet@LinkClose[link];
]

let renderErrorIndicator = (colStart, colEnd, content) => {
  /* TODO:
       why is the colStart = -1
       Reproduce:

       ```
       print_endline("Hello ReasonML folks!")
       let a = 2;
       ```
     */
  let (colStart, pad) =
    if (colStart < 0) {
      (0, abs(colStart));
    } else {
      (colStart, 0);
    };
  String.make(colStart, ' ')
  ++ String.make(
       /* Sometime it reports characters 1-1 */
       switch (colEnd - colStart + pad) {
       | 0 => 1
       | a => a
       },
       '^',
     )
  ++ "\n"
  ++ content;
};

let executeRessultToWidget =
    (
      result: list(ReactTemplate.Worker_Types.final_programEvaluationResult),
    ) => {
  open Worker_Types;
  open Editor_CodeBlockTypes;

  let widgets =
    result
    |. Belt.List.reduceU(
         [||],
         (. acc, exeResult) => {
           let {fn_buffer: _, fn_result: result, fn_pos} = exeResult;
           let (_, {line}) = fn_pos;

           let evaluate =
             result.fn_evaluate
             |. Belt.Option.map(content => Widget.Lw_Value({line, content}));

           let stdout =
             result.fn_stdout
             |. Belt.Option.map(content =>
                  Widget.Lw_Stdout({line, content})
                );

           let stderr =
             switch (result.fn_stderr) {
             | None => [||]
             | Some(errors) =>
               errors
               |. Belt.Array.map(
                    error => {
                      let toWidgetContent = (content: Error.content) => {
                        let (
                          {lno_line, lno_col: colStart},
                          {lno_line: _, lno_col: colEnd},
                        ) =
                          content.pos;

                        {
                          Editor_CodeBlockTypes.Widget.line: lno_line,
                          content:
                            renderErrorIndicator(
                              colStart,
                              colEnd,
                              content.content,
                            ),
                        };
                      };

                      switch (error) {
                      | Err_Warning(content) =>
                        Js.log("warnng");
                        Widget.Lw_Warning(toWidgetContent(content));
                      | Err_Error(content) =>
                        Widget.Lw_Error(toWidgetContent(content))
                      | Err_Unknown(content) =>
                        Widget.Lw_Error({line, content})
                      };
                    },
                  )
             };

           let finalWidgets =
             [|stdout, evaluate|]
             |. Belt.Array.reduceU([||], (. acc2, lineWidget) =>
                  switch (lineWidget) {
                  | None => acc2
                  | Some(lw) => Belt.Array.concat(acc2, [|lw|])
                  }
                );

           Belt.Array.concatMany([|acc, stderr, finalWidgets|]);
         },
       );
  widgets;
};
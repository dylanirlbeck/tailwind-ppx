open Jest;
open Expect;

[@bs.module "../../../js/index"]
external extractor: string => array(string) = "extractor";

describe("Basic test", () => {
  test("basic", () => {
    let className = [%tw "flex-row flex"];
    expect(className) |> toBe("flex-row flex");
  });

  test("should ignore spaces arround", () => {
    // This should not cause compilation errors
    let className = [%tw " flex-row  flex "];

    // And we receive the same string in the output AST
    expect(className) |> toBe(" flex-row  flex ");
  });

  test("Responsive utilities should work", () => {
    let className = [%tw "sm:text-5xl"];

    expect(className) |> toBe("sm:text-5xl");
  });
});

describe("Extractor", () => {
  test("it extracts the content of [%tw] tags properly", () => {
    expect(
      extractor(
        {|
        <!-- This should be included -->
        <div className=[%tw "flex  flex-col bg-gray-200 hover:bg-gray-200"]></div>

        <!-- These shouldn't be included -->
        <div className="flex-row mx-auto mt-2"></div>

        <!-- ...but these should -->
        <div className=[%tw "mb-2 p-6"]></div> <div className=[%tw "mt-8 bg-blue-200"]></div>
        |},
      ),
    )
    |> toEqual([|
         "flex",
         "flex-col",
         "bg-gray-200",
         "hover:bg-gray-200",
         "mb-2",
         "p-6",
         "mt-8",
         "bg-blue-200",
       |])
  })
});

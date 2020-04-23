open Jest;
open Expect;

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
});

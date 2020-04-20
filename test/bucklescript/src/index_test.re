open Jest;
open Expect;

describe("Basic test", () => {
  test("basic", () => {
    let className = [%tw "hover:bg-mono-100 flex"];
    expect(className) |> toBe("hover:bg-mono-100 flex");
  })
});

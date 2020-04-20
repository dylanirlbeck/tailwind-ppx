open Jest;
open Expect;

describe("Basic test", () => {
  test("basic", () => {
    let className = [%tw "mt-2 flex"];
    expect(className) |> toBe("mt-2");
  })
});

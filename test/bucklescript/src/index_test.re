open Jest;
open Expect;

describe("Basic test", () => {
  test("basic", () => {
    let className = [%tw "mt-2"];
    expect(className) |> toBe("mt-2");
  })
});

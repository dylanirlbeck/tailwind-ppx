open Jest;
open Expect;

describe("Basic test", () => {
  test("basic", () => {
    let className = <div className="flex-row flex" />;
    expect(className) |> toBe("flex-row flex");
  })
});

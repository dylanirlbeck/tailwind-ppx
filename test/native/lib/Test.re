open Setup;

describe("Main test ", ({test, _}) =>
  test("basic test", ({expect, _}) => {
    print_string("IN TEST");
    expect.bool(false).toBeTrue();
  })
);

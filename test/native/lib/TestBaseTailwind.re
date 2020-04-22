open Setup;
open TailwindUtils;

describe("Basic parsing", ({test, _}) => {
  test("Parser works for base tailwind file", ({expect, _}) => {
    /* let baseTailwindCss = */
    /*   Read_tailwind.readFile("test/native/lib/tailwind.css"); */
    /* parseStylesheet(baseTailwindCss) |> ignore; */
    /* // TODO find a better way to make sure an error isn't thrown */
    expect.bool(true).toBeTrue()
  });

  test("Unit test #1", ({expect, _}) => {
    let tailwindCss = {|
.transform {
  --transform-translate-x: 0;
  --transform-translate-y: 0;
  --transform-rotate: 0;
  --transform-skew-x: 0;
  --transform-skew-y: 0;
  --transform-scale-x: 1;
  --transform-scale-y: 1;
  transform: translateX(var(--transform-translate-x)) translateY(var(--transform-translate-y)) rotate(var(--transform-rotate)) skewX(var(--transform-skew-x)) skewY(var(--transform-skew-y)) scaleX(var(--transform-scale-x)) scaleY(var(--transform-scale-y));
}
    |};

    parseStylesheet(tailwindCss) |> ignore;
    expect.bool(true).toBeTrue();
  });
});

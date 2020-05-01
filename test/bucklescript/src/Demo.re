[@react.component]
let make = () => {
  <div
    className=[%tw
      "w-full h-full bg-white flex flex-col justify-center
      items-center"
    ]>
    <span> "Content"->React.string </span>
  </div>;
};
